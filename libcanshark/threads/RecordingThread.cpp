#include <iostream>
#include "RecordingThread.h"
#include "Helpers.h"

#define SIZE_THRESHOLD 100

#ifdef _WIN32
#include "winsock2.h"
#else

#include <arpa/inet.h>
#include <QSerialPort>

#endif

namespace dd::libcanshark::threads {

    RecordingThread::RecordingThread(QObject *parent) : QThread(parent) {
        // Register a meta type ???
        typedef QList<dd::libcanshark::data::RecordItem> recordItem;
        qRegisterMetaType<recordItem>("QList<dd::libcanshark::data::RecordItem>&");

        connect(&m_serialPort, &QSerialPort::readyRead,
                this, &RecordingThread::serialDataReady);


    }

    bool RecordingThread::openConnection(const QString &serialPortName) {
        m_serialPortName = serialPortName;

        m_mutex.lock();
        m_serialPort.setPortName(m_serialPortName);
        m_serialPort.setBaudRate(115200);
        m_serialPort.setDataBits(QSerialPort::Data8);
        m_serialPort.setParity(QSerialPort::NoParity);
        m_serialPort.setStopBits(QSerialPort::OneStop);
        m_serialPort.setFlowControl(QSerialPort::NoFlowControl);

        if (m_serialPort.open(QIODevice::ReadWrite)) {
            m_serialPort.setDataTerminalReady(false);
            m_serialPort.setRequestToSend(true);
            m_serialPort.setRequestToSend(m_serialPort.isRequestToSend());
            QThread::msleep(20);
            m_serialPort.setRequestToSend(false);
            m_mutex.unlock();
            emit progressStatus(tr("Connected on %1").arg(serialPortName));
            return true;
        }

        emit finished(tr("Could not connect to serial port %1: %2")
                              .arg(serialPortName, m_serialPort.errorString()));

        m_mutex.unlock();
        return false;
    }

    bool RecordingThread::closeConnection() {
        m_mutex.lock();
        if(!m_serialPort.isOpen())
            return true;

        m_serialPort.close();

        if(m_serialPort.isOpen())
            return false;

        emit progressStatus(tr("Disconnected!"));
        m_mutex.unlock();

        return true;
    }

    bool RecordingThread::startRecording(size_t st_maxMessages) {
        m_mutex.lock();

        st_max_messages = st_maxMessages;

        if (m_serialPort.isOpen()) {
            QThread::msleep(100);
            m_serialPort.write("m");
            m_serialPort.flush();
            b_recording = true;

            emit progressStatus(tr("Started Recording"));

            m_mutex.unlock();

            if (!this->isRunning())
                this->start();

            return true;
        }

        b_recording = false;
        m_mutex.unlock();
        emit finished(tr("Could not start recording! %1").arg(m_serialPort.errorString()));
        return false;
    }

    bool RecordingThread::stopRecording() {
        m_mutex.lock();
        b_recording = false;
        emit progressStatus(tr("Stopped Recording."));
        m_mutex.unlock();
        return true;
    }

    void RecordingThread::run() {
        //1. Open the connection
        while (b_recording) {
            m_mutex.lock();
            for (auto &packet: m_packetHexStringsList) {
                auto packetHexData = helpers::Helpers::hex2bytes<uint8_t>(packet.toStdString());

                if (packetHexData.empty())
                    continue;

                std::reverse(packetHexData.begin(), packetHexData.end());

                // Get the progressMessage length
                uint32_t messageLength = 0;
                memcpy(&messageLength, packetHexData.data(), sizeof(uint32_t));
                messageLength = ntohl(messageLength);

                if (messageLength > 32 || messageLength <= 0)
                    continue; //Bad message size;

                // Get the us delta time
                uint32_t usDeltaTime = 0;
                memcpy(&usDeltaTime, packetHexData.data() + sizeof(uint32_t), sizeof(uint32_t));
                usDeltaTime = ntohl(usDeltaTime);

                // Get the type
                uint16_t type = 0;
                memcpy(&type, packetHexData.data() + sizeof(uint32_t) + sizeof(uint32_t), sizeof(uint16_t));
                type = ntohs(type);

                // Get the ID
                uint32_t id = 0;
                memcpy(&id, packetHexData.data() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t),
                       sizeof(uint32_t));
                id = ntohl(id);

                size_t canDataLength = messageLength - (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t));

                if (canDataLength > 8)
                    continue; //Bad can data length

                auto *canData = (uint8_t *) malloc(sizeof(uint8_t) * canDataLength);
                memcpy(canData, packetHexData.data() + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t),
                       canDataLength);

                uint16_t crc16 = 0;
                memcpy(&crc16, packetHexData.data() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) +
                               sizeof(uint32_t) + canDataLength, sizeof(uint16_t));
                crc16 = ntohs(crc16);

                //1. Convert hex string to a record item
                dd::libcanshark::data::RecordItem recordItem = {
                        .total_size = (uint32_t) messageLength,
                        .type = static_cast<dd::libcanshark::data::CanFrameType>(type),
                        .time = usDeltaTime,
                        .id = id,
                        .crc16 = crc16
                };

                recordItem.canDataLength = canDataLength;
                recordItem.data = (uint8_t *) malloc(sizeof(uint8_t) * canDataLength);
                memcpy(recordItem.data, canData, canDataLength);

                free(canData);

                //Remove the hex string from the list
                m_packetHexStringsList.removeIf([&packet](const QString &t) -> bool {
                    return t == packet;
                });

                m_canPackets.emplace_back(recordItem);
            }

            if (m_canPackets.size() > SIZE_THRESHOLD) {
                emit dataReady(m_canPackets);
                m_canPackets.clear();
            }
            m_mutex.unlock();
        }

        m_mutex.lock();
        m_serialPort.write("n");
        m_serialPort.flush();
        m_mutex.unlock();
    }

    void RecordingThread::serialDataReady() {
        QByteArray response = m_serialPort.readAll();

        if (response.size() <= 0) {
            return;
        }

        emit progressStatus(tr("Received %1bytes").arg(response.size()));

        if (st_max_messages > 0 && st_recorded_message_count == st_max_messages)
            return;

        for (char c: response.toStdString()) {
            if (c == '<') {
                b_appendToPacket = true;
                continue;
            } else if (c == '>') {
                b_appendToPacket = false;
                m_mutex.lock();
                m_packetHexStringsList.append(m_packetHexString);
                m_mutex.unlock();
                m_packetHexString.clear();
            }

            if (b_appendToPacket) {
                m_packetHexString.append(c);
            }
        }
        st_recorded_message_count++;
    }
} // threads
#include "FirmwareUpdateThread.h"
#include "Helpers.h"

#include <iostream>

#include <QThread>
#include <QFile>
#include <utility>

#ifdef _WIN32
#include "winsock2.h"
#else
#include <arpa/inet.h>
#endif

namespace dd::libcanshark::threads {
    FirmwareUpdateThread::FirmwareUpdateThread(QString fileName, QString portName)
        : m_fileName(std::move(fileName)), m_serialPortName(std::move(portName))
    { }

    /**
     * The thread loop
     */
    void FirmwareUpdateThread::run() {
        mutex.lock();
        if(m_serial == nullptr)
            m_serial = new QSerialPort();

        openConnection();

        //Check if the serial port is connected, if not open it, if it can't open, fail and bail
        if(!m_serial->isOpen())
            if(!m_serial->open(QIODevice::ReadWrite)){
                emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open serial port!"));
                mutex.unlock();
                return;
            }


        //Load the firmware update file
        QFile firmwareUpdateFile(this->m_fileName);

        //Open the firmware update file
        if(!firmwareUpdateFile.open(QFile::OpenModeFlag::ReadOnly))
        {
            emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open firmware update file!"));
            mutex.unlock();
            return;
        }

        //Send the canshark mini the update byte
        this->m_serial->write("u"); //Put in update mode

        //Get the update size
        size_t update_size = htonl(firmwareUpdateFile.size());

        //Write to the can shark mini the size of the update
        this->m_serial->write(reinterpret_cast<const char *>(&update_size), sizeof(size_t));

        this->m_serial->flush();

        //Wait for update mode to kick in
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

        QByteArray file_data = firmwareUpdateFile.readAll();
        QList<QByteArray> lines;

        helpers::Helpers::split(file_data, lines, 512);

        size_t progress_count = 0;

        for(const auto& line : lines) {
            size_t written_count = this->m_serial->write(line);
            this->m_serial->waitForBytesWritten(1000);

            this->m_serial->flush();

            progress_count += written_count;

            emit progressMessage(tr("Wrote: %1 [%2 of %3]")
                                         .arg(written_count)
                                         .arg(progress_count)
                                         .arg(file_data.size()));

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        closeConnection();
        mutex.unlock();

        emit finished(FirmwareUpdateThreadStatus::Success, tr("Update complete!"));
    }

    bool FirmwareUpdateThread::openConnection() {
        if (m_serialPortName.isEmpty()) {
            return false;
        }

        if (m_serial->isOpen())
            closeConnection();

        m_serial->setPortName(m_serialPortName);
        m_serial->setBaudRate(115200);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            m_serial->setDataTerminalReady(false);
            m_serial->setRequestToSend(true);
            m_serial->setRequestToSend(m_serial->isRequestToSend());
            QThread::msleep(20);
            m_serial->setRequestToSend(false);
            mutex.unlock();
            return true;
        }

        return false;
    }

    bool FirmwareUpdateThread::closeConnection() {
        m_serial->setDataTerminalReady(true);
        m_serial->setRequestToSend(false);
        m_serial->close();

        return true;
    }
} // threads
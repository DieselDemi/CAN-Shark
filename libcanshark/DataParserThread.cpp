#include <iostream>
#include "DataParserThread.h"

#define SIZE_THRESHOLD 100

namespace dd::libcanshark::threads {

    DataParserThread::DataParserThread(QObject *parent)
        : QThread(parent)
    {}

    DataParserThread::~DataParserThread()
    {
        mutex.lock();
        running = true;
        mutex.unlock();
        wait();
    }

    void DataParserThread::init(SerialThread& serialThread)
    {
        QMutexLocker locker(&mutex);

        connect(&serialThread, &dd::libcanshark::threads::SerialThread::response,
                this, &dd::libcanshark::threads::DataParserThread::serialDataReceived);

        if(!isRunning())
        {
            running = true;
            start(HighestPriority);
        }
    }

    void DataParserThread::run() {
        while (running) {
            mutex.lock();

//            if(packetHexStrings.size() > SIZE_THRESHOLD)
//                continue;

            for (auto &packet: packetHexStrings) {
                QList<QString> byteStrings;

                assert(packet.size() % 2 == 0);

                for (qsizetype i = 0; i < packet.size(); i += 2) {
                    QString byteString = packet.mid(i, 2);
                    byteStrings.append(byteString);
                }

                if (byteStrings.empty())
                    continue;

                QString lengthString;

                lengthString = byteStrings[3] +
                               byteStrings[2] +
                               byteStrings[1] +
                               byteStrings[0];

                bool bConvertedOk = false;

                size_t length = lengthString.toULong(&bConvertedOk, 16);
                assert(bConvertedOk);

                auto dataLen = (qsizetype) (length - sizeof(uint16_t));
                uint8_t packetData[dataLen];

                for (qsizetype i = 0; i < dataLen; i++) {
                    bConvertedOk = false;
                    packetData[dataLen - i] = (uint8_t) byteStrings[4 + i].toUShort(&bConvertedOk, 16);
                    assert(bConvertedOk);
                }

                QString crcString = byteStrings[4 + dataLen + 1] +
                                    byteStrings[4 + dataLen];

                bConvertedOk = false;
                uint16_t crc = crcString.toUShort(&bConvertedOk, 16);
                assert(bConvertedOk);

                uint64_t time = 0;
                uint16_t type = 0;
                uint32_t id = 0;
                size_t canDataLen = dataLen - 8 + 2 + 4;
                auto *canData = (uint8_t *) malloc(sizeof(uint8_t) * canDataLen);

                memcpy(&time, packetData, 8);
                memcpy(&type, packetData + 9, 2);
                memcpy(&id, packetData + 11, 4);
                memcpy(&canData, packetData + 15, dataLen);

                //1. Convert hex string to a record item
                dd::libcanshark::data::RecordItem data = {
                        .total_size = (uint32_t) dataLen,
                        .type = static_cast<dd::libcanshark::data::CanFrameType>(type),
                        .time = time,
                        .id = id,
                        .data = canData,
                        .crc16 = crc
                };

                //Remove the hex string from the list
                packetHexStrings.removeIf([&packet](const QString& t)->bool {
                   return t == packet;
                });

                packets.emplace_back(data);
            }
            mutex.unlock();

            if(packets.size() > SIZE_THRESHOLD)
            {
                emit dataReady(packets);
                packets.clear();
            }
        }
    }

    void DataParserThread::serialDataReceived(const QString &data) {
        for (char c: data.toStdString()) {
            if (c == '<') {
                bAppendToPacket = true;
                continue;
            } else if (c == '>') {
                bAppendToPacket = false;
                mutex.lock();
                packetHexStrings.append(packetHexString);
                mutex.unlock();
                packetHexString.clear();
            }

            if (bAppendToPacket) {
                packetHexString.append(c);
            }
        }
    }
} // threads
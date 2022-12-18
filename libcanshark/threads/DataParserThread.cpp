#include <iostream>
#include "DataParserThread.h"
#include "Helpers.h"

#define SIZE_THRESHOLD 100

#ifdef _WIN32
#include "winsock2.h"
#else
#include <arpa/inet.h>
#endif

namespace dd::libcanshark::threads {

    DataParserThread::DataParserThread(QObject *parent) : QThread(parent)
    {
        // Register a meta type ???
        typedef QList<dd::libcanshark::data::RecordItem> recordItem;
        qRegisterMetaType<recordItem>("QList<dd::libcanshark::data::RecordItem>&");
        init();
    }

    DataParserThread::~DataParserThread()
    {
        mutex.lock();
        running = true;
        mutex.unlock();
        wait();
    }

    void DataParserThread::init()
    {
        QMutexLocker locker(&mutex);

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
                auto packetHexData = helpers::Helpers::hex2bytes<uint8_t>(packet.toStdString());

                if(packetHexData.empty())
                    continue;

                std::reverse(packetHexData.begin(), packetHexData.end());

                // Get the progressMessage length
                uint32_t messageLength = 0;
                memcpy(&messageLength, packetHexData.data(), sizeof(uint32_t));
                messageLength = ntohl(messageLength);

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
                memcpy(&id, packetHexData.data() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t), sizeof(uint32_t));
                id = ntohl(id);

                size_t canDataLength = messageLength - (sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t));
                auto* canData = (uint8_t*)malloc(sizeof(uint8_t) * canDataLength);
                memcpy(canData, packetHexData.data() + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t), canDataLength);

                uint16_t crc16 = 0;
                memcpy(&crc16, packetHexData.data() + sizeof(uint32_t) + sizeof(uint32_t) + sizeof(uint16_t) + sizeof(uint32_t) + canDataLength, sizeof(uint16_t));
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
                recordItem.data = (uint8_t*)malloc(sizeof(uint8_t) * canDataLength);
                memcpy(recordItem.data, canData, canDataLength);

                free(canData);

                //Remove the hex string from the list
                packetHexStrings.removeIf([&packet](const QString& t)->bool {
                   return t == packet;
                });

                packets.emplace_back(recordItem);
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
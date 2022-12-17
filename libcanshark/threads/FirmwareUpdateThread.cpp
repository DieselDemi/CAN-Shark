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
    FirmwareUpdateThread::FirmwareUpdateThread(QString fileName, QSerialPort *serialPort)
        : m_fileName(std::move(fileName)), m_serial(serialPort)
    { }

    void FirmwareUpdateThread::run() {
        assert(m_serial != nullptr);

        //Check if the serial port is connected, if not open it, if it can't open, fail and bail
        if(!m_serial->isOpen())
            if(!m_serial->open(QIODevice::ReadWrite)){
                emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open serial port!"));
                return;
            }


        //Load the firmware update file
        QFile firmwareUpdateFile(this->m_fileName);

        //Open the firmware update file
        if(!firmwareUpdateFile.open(QFile::OpenModeFlag::ReadOnly))
        {
            emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open firmware update file!"));
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

        emit finished(FirmwareUpdateThreadStatus::Success, tr("Update complete!"));
    }
} // threads
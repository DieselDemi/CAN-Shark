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
        if (m_serialPortName.isEmpty()) {
            emit finished(FirmwareUpdateThreadStatus::Fail, tr("Serial port name is empty"));
            return;
        }

        auto serialPort = QSerialPort();

        serialPort.setPortName(m_serialPortName);
        serialPort.setBaudRate(115200);
        serialPort.setDataBits(QSerialPort::Data8);
        serialPort.setParity(QSerialPort::NoParity);
        serialPort.setStopBits(QSerialPort::OneStop);
        serialPort.setFlowControl(QSerialPort::NoFlowControl);

        if (serialPort.open(QIODevice::ReadWrite)) {
            serialPort.setDataTerminalReady(false);
            serialPort.setRequestToSend(true);
            serialPort.setRequestToSend(serialPort.isRequestToSend());
            QThread::msleep(20);
            serialPort.setRequestToSend(false);
        } else {
            emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open serial port!"));
            return;
        }

        //Check if the serial port is connected, if not open it, if it can't open, fail and bail
        if(!serialPort.isOpen())
            if(!serialPort.open(QIODevice::ReadWrite)){
                emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open serial port!"));
                return;
            }

        //Load the firmware update file
        QFile firmwareUpdateFile(this->m_fileName);

        //Open the firmware update file
        if(!firmwareUpdateFile.open(QFile::OpenModeFlag::ReadOnly))
        {
            emit finished(FirmwareUpdateThreadStatus::Fail, tr("Could not open firmware update file!"));
            exit();
            return;
        }

        //Send the canshark mini the update byte
        serialPort.write("u"); //Put in update mode
        //Get the update size
        size_t update_size = htonl(firmwareUpdateFile.size());
        //Write to the can shark mini the size of the update
        serialPort.write(reinterpret_cast<const char *>(&update_size), sizeof(size_t));
        //Flush the data to the buffer
        serialPort.flush();
        //Wait for update mode to kick in
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        //Read the update file
        QByteArray file_data = firmwareUpdateFile.readAll();
        //Dedicate chunks to the update file
        QList<QByteArray> lines;
        helpers::Helpers::split(file_data, lines, 512);

        //Set up a progress counter
        size_t progress_count = 0;

        for(const auto& line : lines) {
            size_t written_count = serialPort.write(line);
            serialPort.waitForBytesWritten(1000);

            serialPort.flush();

            progress_count += written_count;

            emit progressMessage(tr("Wrote: %1 [%2 of %3]")
                                         .arg(written_count)
                                         .arg(progress_count)
                                         .arg(file_data.size()));

            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        serialPort.setDataTerminalReady(true);
        serialPort.setRequestToSend(false);
        serialPort.close();

        emit finished(FirmwareUpdateThreadStatus::Success, tr("Update complete!"));
    }

} // threads
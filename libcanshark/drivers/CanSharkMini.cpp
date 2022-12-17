#include "CanSharkMini.h"

#include <QThread>
#include <QFile>
#include <Helpers.h>

#include <iostream>

#ifdef _WIN32
#include "winsock2.h"
#else
#include <arpa/inet.h>
#endif


namespace dd::libcanshark::drivers {

    CanSharkMini::CanSharkMini(
            libcanshark::threads::DataParserThread *dataThread,
            QObject *parent) : CanShark(dataThread, parent) {}

    /**
     * Open the serial port, closes the port if its currently open
     * @return success
     */
    bool CanSharkMini::openConnection(QString const &portName) {
        if (portName.isEmpty()) {
            emit statusMessage(tr("Select a device to connect to first"));
            return false;
        }

        if (m_serial->isOpen())
            closeConnection();

        m_serial->setPortName(portName);
        m_serial->setBaudRate(115200);
        m_serial->setDataBits(QSerialPort::Data8);
        m_serial->setParity(QSerialPort::NoParity);
        m_serial->setStopBits(QSerialPort::OneStop);
        m_serial->setFlowControl(QSerialPort::NoFlowControl);

        if (m_serial->open(QIODevice::ReadWrite)) {
            emit statusMessage(tr("Connected to CanShark Mini on %1").arg(portName));

            m_serial->setDataTerminalReady(false);
            m_serial->setRequestToSend(true);
            m_serial->setRequestToSend(m_serial->isRequestToSend());
            QThread::msleep(20);
            m_serial->setRequestToSend(false);

            return true;
        } else {
            emit errorMessage(tr("Open error %1").arg(m_serial->errorString()));
            return false;
        }

    }

    /**
     * Closes the serial port if the port is open
     * @return success
     */
    bool CanSharkMini::closeConnection() {
        if (m_serial->isOpen()) {
            if (b_recording) {
                if (!stopRecording()) {
                    return false;
                }
            }

            m_serial->setDataTerminalReady(true);
            m_serial->setRequestToSend(false);
            m_serial->close();
        }

        if (m_serial->isOpen())
            return false;

        emit statusMessage(tr("Disconnected"));

        return true;
    }

    /**
     * Sends the start recording command to the CANShark Mini
     * @return success
     */
    bool CanSharkMini::startRecording(size_t max_messages) {
        this->st_max_messages = max_messages;

        if (this->m_serial->isOpen()) {
            m_serial->write("m");
            if (m_serial->waitForBytesWritten()) {
                emit statusMessage(tr("Started recording"));
                return true;
            }
        }
        emit errorMessage(tr("Could not start recording!"));
        return false;
    }

    /**
     * Sends the stop recording command to the CANShark Mini
     * @return success
     */
    bool CanSharkMini::stopRecording() {
        if (this->m_serial->isOpen()) {
            m_serial->write("n");
            if (m_serial->waitForBytesWritten()) {
                emit statusMessage(tr("Stopped recording"));
                return true;
            }
        }
        emit errorMessage(tr("Could not stop recording!"));
        return false;
    }

    /**
     * Send a firmware update to the CanSharkMini
     * @param firmwareUpdateFileName
     * @return
     */
    bool CanSharkMini::updateFirmware(const QString &firmwareUpdateFileName) {
        if(this->m_serial->isOpen()) {
            disconnect(m_serial, &QSerialPort::readyRead, this, &CanSharkMini::readData);

            this->m_updateMode = true;

            QFile firmwareUpdateFile(firmwareUpdateFileName);

            //Open the firmware update file
            if(!firmwareUpdateFile.open(QFile::OpenModeFlag::ReadOnly))
                return false;

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

                emit statusMessage(tr("Wrote: %1 [%2 of %3]")
                    .arg(written_count)
                    .arg(progress_count)
                    .arg(file_data.size()));

                std::cout << "Wrote: " << written_count << "bytes [" << progress_count << " of " << file_data.size() << "]" << std::endl;

                std::this_thread::sleep_for(std::chrono::milliseconds(100));

                //Wait till we recieve a response from the CSM
//                QByteArray response = this->m_serial->readAll();
//                while(response.size() == 0) {
//                    response = this->m_serial->readAll();
//                    if(response.size() != 0)
//                        std::cout << "CSM Responded: " << response.toStdString() << std::endl;
//                }
            }

            emit statusMessage("Update complete");
            this->m_updateMode = false;

            connect(m_serial, &QSerialPort::readyRead, this, &CanSharkMini::readData);

            return true;
        }
        emit errorMessage(tr("Must be connected to device first!"));
        return false;
    }


} // drivers
#include "CanSharkMini.h"

#include <QThread>

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
        //        if(port.isOpen()) {
//            auto file_name =
//                    QFileDialog::getOpenFileName(
//                            this,
//                            tr("Select Update"),
//                            "/home",
//                            tr("Update Files (*.csu)"));
//
//            QFile input(file_name);
//
//            if(!input.open(QFile::OpenModeFlag::ReadOnly))
//                return;
//
//            port.write(QByteArray::fromStdString("u")); //Put in update mode
//
//            qint64 update_size = input.size();
//
//            port.write(reinterpret_cast<const char *>(&update_size), sizeof(qint64));
//
//            port.flush();
//
//            //Wait for update mode to kick in
//            std::this_thread::sleep_for(std::chrono::milliseconds(100));
//
//            QByteArray file_data = input.readAll();
//            QList<QByteArray> lines;
//
//            split(file_data, lines, 512);
//
//            qint64 progress_count = 0;
//
//            for(const auto& line : lines) {
//                qint64 written_count = port.write(line);
//                port.flush();
//
//                progress_count += written_count;
//
//                std::cout << "Wrote: " << written_count << " bytes" << std::endl;
//                std::cout << progress_count << " of " << file_data.size() << std::endl;
//
//                port.waitForReadyRead(100);
//                auto data = port.readAll();
//
//                std::cout << "Read: " << data.size() << std::endl;
//            }
//
//            while(!input.atEnd()) {
//
//                QByteArray line = input.readLine();
//                qint64 written_count = port.write(line);
//
//                std::cout << line.size() << " == " << written_count << std::endl;
//                assert(line.size() == written_count);
//
//                port.flush();
//
//                while(port.bytesToWrite() != 0){
//                    port.waitForBytesWritten(10);
//                }
//
//                if(!port.waitForBytesWritten(100)) {
//                    std::cout << "Error writting bytes" << std::endl;
//                }
//            }
//
//            for(const auto& byte : file_data.toStdString()) {
//                port.write(reinterpret_cast<const char *>(byte));
//                port.
//                auto read_data = port.readAll();
//
//                for(const auto& read_byte : read_data) {
//                    std::cout << std::hex << read_byte << " ";
//                }
//                std::cout << std::endl;
//            }
//
//
//            port.write(file_data);
//
//        }

        return false;
    }


} // drivers
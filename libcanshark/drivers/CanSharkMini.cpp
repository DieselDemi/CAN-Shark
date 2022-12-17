#include "CanSharkMini.h"

#include <QThread>
#include <QFile>
#include <Helpers.h>

#include <iostream>


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
        if (!this->m_serial->isOpen()) {
            emit errorMessage(tr("Connect to the CAN Shark Mini first!"));
            return false;
        }

        if (ptr_firmwareUpdateThread == nullptr)
        {
            ptr_firmwareUpdateThread = new threads::FirmwareUpdateThread(firmwareUpdateFileName, this->m_serial);

            connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::finished, this,
                    &CanSharkMini::updateThreadFinished);

            connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::progressMessage, this,
                    &CanSharkMini::updateThreadProgress);

            ptr_firmwareUpdateThread->start();
            emit statusMessage("Firmware update started");
            return true;
        }

        if (ptr_firmwareUpdateThread->isFinished()) {
            connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::finished, this,
                    &CanSharkMini::updateThreadFinished);

            connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::progressMessage, this,
                    &CanSharkMini::updateThreadProgress);

            ptr_firmwareUpdateThread->start();
            emit statusMessage("Firmware update started");
            return true;
        }

        return false;
    }

    void CanSharkMini::updateThreadFinished(threads::FirmwareUpdateThreadStatus status, const QString &message) {
        switch (status) {
            case threads::FirmwareUpdateThreadStatus::Success: {
                emit statusMessage(tr("Firmware update complete!"));
                emit updateComplete(threads::FirmwareUpdateThreadStatus::Success);
                break;
            }
            case threads::FirmwareUpdateThreadStatus::Fail: {
                emit errorMessage(tr("Error updating firmware!!!: %1").arg(message));
                emit updateComplete(threads::FirmwareUpdateThreadStatus::Fail);
                break;
            }
        }

        disconnect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::finished, this,
                   &CanSharkMini::updateThreadFinished);
        disconnect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::progressMessage, this,
                &CanSharkMini::updateThreadProgress);
    }

    void CanSharkMini::updateThreadProgress(const QString &message) {
        emit statusMessage(message);
    }
} // drivers
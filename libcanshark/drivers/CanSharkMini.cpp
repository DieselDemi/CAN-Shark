#include "CanSharkMini.h"

#include <QThread>
#include <Helpers.h>

#include <iostream>


namespace dd::libcanshark::drivers {

    CanSharkMini::CanSharkMini(QObject *parent) : CanShark(parent) {}

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
    bool CanSharkMini::updateFirmware(const QString &firmwareUpdateFileName, const QString& selectedDevicePortName) {
        if (this->m_serial->isOpen()) {
            this->m_serial->close();
        }

        this->m_serialPortName = selectedDevicePortName;

        if (ptr_firmwareUpdateThread == nullptr) {
            ptr_firmwareUpdateThread = new threads::FirmwareUpdateThread(firmwareUpdateFileName, selectedDevicePortName);
            startFirmwareUpdate();
            return true;
        } else if (ptr_firmwareUpdateThread->isFinished()) {
            startFirmwareUpdate();
            return true;
        }

        return false;
    }

    /**
     *
     * @param status
     * @param message
     */
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

        delete ptr_firmwareUpdateThread;
    }

    /**
     *
     * @param message
     */
    void CanSharkMini::updateThreadProgress(const QString &message) {
        emit statusMessage(message);
    }

    /**
     * Wrapper function for the firmware update
     */
    void CanSharkMini::startFirmwareUpdate() {
        connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::finished, this,
                &CanSharkMini::updateThreadFinished);

        connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::progressMessage, this,
                &CanSharkMini::updateThreadProgress);

        ptr_firmwareUpdateThread->start();

        emit statusMessage("Firmware update started");
    }
} // drivers
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
    bool CanSharkMini::startRecording(const QString& serialPortName, size_t max_messages) {
        this->m_serialPortName = serialPortName;

        if (ptr_recordingThread == nullptr) {
            ptr_recordingThread = new threads::RecordingThread();

            connect(ptr_recordingThread, &threads::RecordingThread::dataReady,
                    this, &CanSharkMini::dataReady);
            connect(ptr_recordingThread, &threads::RecordingThread::finished,
                    this, &CanSharkMini::recordingThreadFinished);

        } else if (ptr_recordingThread->isFinished()) {
            ptr_recordingThread->start();
        }

        return this->ptr_recordingThread->startRecording(this->m_serialPortName, max_messages);
    }

    /**
     * Sends the stop recording command to the CANShark Mini
     * @return success
     */
    bool CanSharkMini::stopRecording() {
        return this->ptr_recordingThread->stopRecording();
    }

    /**
     * Send a firmware update to the CanSharkMini
     * @param firmwareUpdateFileName
     * @return
     */
    bool CanSharkMini::updateFirmware(const QString &firmwareUpdateFileName, const QString &selectedDevicePortName) {
        this->m_serialPortName = selectedDevicePortName;

        if (ptr_firmwareUpdateThread == nullptr) {
            ptr_firmwareUpdateThread = new threads::FirmwareUpdateThread(firmwareUpdateFileName,
                                                                         selectedDevicePortName);
            startFirmwareUpdate();
            return true;
        } else if (ptr_firmwareUpdateThread->isFinished()) {
            startFirmwareUpdate();
            return true;
        }

        return false;
    }

    /**
     * Start the firmware update thread
     */
    void CanSharkMini::startFirmwareUpdate() {
        connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::finished, this,
                &CanSharkMini::updateThreadFinished);

        connect(ptr_firmwareUpdateThread, &threads::FirmwareUpdateThread::progressMessage, this,
                &CanSharkMini::updateThreadProgress);

        ptr_firmwareUpdateThread->start();

        emit statusMessage("Firmware update started");
    }

    /**
     * Fired when the update thread has a progress update
     * @param message
     */
    void CanSharkMini::updateThreadProgress(const QString &message) {
        emit statusMessage(message);
    }

    /**
     * Fired when the update thread is finished!.
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

    void CanSharkMini::recordingThreadFinished(const QString &message) {
        emit statusMessage(message);
    }
} // drivers
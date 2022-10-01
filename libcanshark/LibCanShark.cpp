#include "LibCanShark.h"

#include <QtSerialPort/QSerialPort>
#include <QTime>
#include <iostream>

LibCanShark::LibCanShark(QObject *parent) : QThread(parent) {}

LibCanShark::~LibCanShark() {
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

void LibCanShark::connect(const QString &portName, int waitTimeout) {
    QMutexLocker locker(&mutex);

    this->portname = portName;
    this->waitTimeout = waitTimeout;

    if(!isRunning())
    {
        start();
        emit message(tr("Connected"));
    }
    else
        emit message(tr("Already connected"));
}

void LibCanShark::run() {
    QSerialPort serial;

    bool currentPortNameChanged = false;

    mutex.lock();
    QString currentPortName;
    if (currentPortName != portname) {
        currentPortName = portname;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    mutex.unlock();

    emit message("Serial connection thread started");

    while (!quit) {
        if (!connected) {
//            serial.close();
            serial.setPortName(currentPortName);
            serial.setBaudRate(115200, QSerialPort::AllDirections);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                                   .arg(portname).arg(serial.error()));

                connected = false;
                return;
            }

            connected = true;
        }

        if(bStartRecording) {
            serial.write("m");
            if(serial.waitForBytesWritten(waitTimeout)) {
                emit message(tr("Serial starting to record"));
                bRecording = true;
                bStartRecording = false;
                bStopRecording = false;
            } else {
                emit error(tr("Could not start bRecording"));
                bRecording = false;
                bStopRecording = true;
                bStartRecording = false;
            }
        } else if(bStopRecording) {
            serial.write("n");
            if(serial.waitForBytesWritten(waitTimeout)) {
                emit message(tr("Stopped recording"));
                bRecording = false;
                bStartRecording = false;
                bStopRecording = false;
            } else {
                emit error(tr("Could not stop recording"));
                bRecording = false;
                bStartRecording = false;
                bStopRecording = false;
            }
        }

        if(bRecording && connected) {
            if(serial.waitForReadyRead(currentWaitTimeout)){
                this->responseData = serial.readAll();
                emit response(this->responseData);
            }
//            else {
//                serial.write("m\0");
//                if(serial.waitForBytesWritten(waitTimeout)) {
//                    emit message(tr("Sent M to device"));
//                } else {
//                    emit error(tr("Could not resend M"));
//                }
//            }
        }

        mutex.lock();
        if (currentPortName != portname) {
            currentPortName = portname;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        mutex.unlock();
    }
}

void LibCanShark::disconnect() {
    mutex.lock();
    this->quit = true;
    emit message(tr("Disconnected..."));
    mutex.unlock();
}

void LibCanShark::startRecording() {
    if(!connected)
        emit error(tr("Not currently connected"));

    mutex.lock();
    bStartRecording = true;
    mutex.unlock();
}

void LibCanShark::stopRecording() {
    if(!connected)
        emit error(tr("Not currently connected"));

    mutex.lock();
    bStopRecording = true;
    mutex.unlock();
}

void LibCanShark::sendFirmwareUpdate(QByteArray& data) {
    //TODO
}

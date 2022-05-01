#include "LibCanShark.h"

#include <QtSerialPort/QSerialPort>
#include <QTime>

LibCanShark::LibCanShark(QObject *parent) : QThread(parent) {

}

LibCanShark::~LibCanShark() {
    mutex.lock();
    quit = true;
    mutex.unlock();
    wait();
}

void LibCanShark::connect(const QString &portName, int waitTimeout, const QString &response) {
    QMutexLocker locker(&mutex);

    this->portname = portName;
    this->waitTimeout = waitTimeout;
    this->response = response;

    if(!isRunning())
        start();
}

void LibCanShark::run() {
    bool currentPortNameChanged = false;

    mutex.lock();
    QString currentPortName;
    if (currentPortName != portname) {
        currentPortName = portname;
        currentPortNameChanged = true;
    }

    int currentWaitTimeout = waitTimeout;
    QString currentRespone = response;
    mutex.unlock();
    QSerialPort serial;

    while (!quit) {
        if (currentPortNameChanged) {
            serial.close();
            serial.setPortName(currentPortName);

            if (!serial.open(QIODevice::ReadWrite)) {
                emit error(tr("Can't open %1, error code %2")
                                   .arg(portname).arg(serial.error()));
                return;
            }
        }

        if (serial.waitForReadyRead(currentWaitTimeout)) {
            // read request
            QByteArray requestData = serial.readAll();
            while (serial.waitForReadyRead(10))
                requestData += serial.readAll();
            // write response
            QByteArray responseData = currentRespone.toLocal8Bit();
            serial.write(responseData);
            if (serial.waitForBytesWritten(waitTimeout)) {
                QString request(requestData);
                emit this->request(request);
            } else {
                emit timeout(tr("Wait write response timeout %1")
                                     .arg(QTime::currentTime().toString()));
            }
        } else {
            emit timeout(tr("Wait read request timeout %1")
                                 .arg(QTime::currentTime().toString()));
        }
        mutex.lock();
        if (currentPortName != portname) {
            currentPortName = portname;
            currentPortNameChanged = true;
        } else {
            currentPortNameChanged = false;
        }
        currentWaitTimeout = waitTimeout;
        currentRespone = response;
        mutex.unlock();
    }
}

void LibCanShark::error(const QString &s) {

}

void LibCanShark::request(const QString &s) {

}

void LibCanShark::timeout(const QString &s) {

}

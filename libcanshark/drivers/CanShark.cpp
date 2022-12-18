#include "CanShark.h"
#include <QSerialPortInfo>
#include <iostream>
#include <DataParserThread.h>

namespace dd::libcanshark::drivers {

    /// Constructors && Destructors
    CanShark::CanShark(QObject *parent) : QObject(parent) {
        this->m_serial = new QSerialPort();
        this->m_dataThread = new threads::DataParserThread();

        //Connect serial events
        connect(m_serial, &QSerialPort::errorOccurred, this, &CanShark::serialError);
        connect(m_serial, &QSerialPort::readyRead, this, &CanShark::readData);

        assert(m_dataThread != nullptr);

        //Connect the thread signal
        connect(this, &CanShark::serialDataReceived, m_dataThread,
                &libcanshark::threads::DataParserThread::serialDataReceived);
    }

    CanShark::~CanShark() {
        //TODO Fix this
//        if (m_serial->isOpen())
//            m_serial->close();
    }

    /// Slots
    /**
     * Called when QSerialPort fires the correct dataAvailable event
     */
    void CanShark::readData() {
        QByteArray response = m_serial->readAll();
        if (response.size() <= 0)
            return;

        if (m_updateMode) {
            return;
        } else {
            if (st_max_messages > 0 && st_recorded_message_count == st_max_messages)
                return;

            emit statusMessage(tr("Received Message of %1bytes").arg(response.size()));
            emit serialDataReceived(response);

            st_recorded_message_count++;
        }
    }

    /**
     * Called when QSerialPort fires the SerialPortError event
     * @param error
     */
    void CanShark::serialError(QSerialPort::SerialPortError error) {
        if (error == QSerialPort::ResourceError) {
            emit errorMessage(tr("%1").arg(m_serial->errorString()));
            closeConnection();
        }
    }

    /**
     * Get the available ports on the system
     * @return List of all the available ports
     */
    QList<std::tuple<QString, QString>> CanShark::getAvailablePorts() {
        auto *_ret = new QList<std::tuple<QString, QString>>();

        for (const auto &serial_port: QSerialPortInfo::availablePorts()) {
#ifdef _DEBUG
            std::cout << serial_port.portName().toStdString() << " "
                      << serial_port.description().toStdString() << " "
                      << serial_port.manufacturer().toStdString() << " "
                      << serial_port.serialNumber().toStdString() << " "
                      << std::endl;
#endif
            //TODO: Find the name on linux distros, below is windows and mac
            // Also differentiate between canshark versions
            if ((serial_port.serialNumber() == "CANSHARKMINI" ||
                 serial_port.portName().contains("cu.SLAB_USBtoUART")) &&
                !serial_port.portName().contains("CANSHARKMINI")) {
                QString name = tr("%1 %2")
                        .arg("CAN Shark Mini on", serial_port.portName());

                _ret->push_back({name, serial_port.portName()});
            }
        }

        return *_ret;
    }

    threads::DataParserThread *CanShark::getDataParserThread() {
        return this->m_dataThread;
    }

    /**
     * Open the serial port, closes the port if its currently open
     * @return success
     */
    bool CanShark::openConnection(const QString &portName) {
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
    bool CanShark::closeConnection() {
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


} // drivers
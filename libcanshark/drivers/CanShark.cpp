#include "CanShark.h"
#include <QSerialPortInfo>
#include <iostream>
#include <DataParserThread.h>

namespace dd::libcanshark::drivers {

    /// Constructors && Destructors
    CanShark::CanShark(libcanshark::threads::DataParserThread *dataThread,
                       QObject *parent) : QObject(parent),
                                          m_dataThread(dataThread),
                                          m_serial(new QSerialPort(this)) {

        //Connect serial events
        connect(m_serial, &QSerialPort::errorOccurred, this, &CanShark::serialError);
        connect(m_serial, &QSerialPort::readyRead, this, &CanShark::readData);

        assert(m_dataThread != nullptr);

        //Connect the thread signal
        connect(this, &CanShark::serialDataReceived, dataThread,
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
        if(response.size() <= 0)
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
            if ((serial_port.serialNumber() == "CANSHARKMINI" || serial_port.portName().contains("cu.SLAB_USBtoUART")) && !serial_port.portName().contains("CANSHARKMINI")) {
                QString name = tr("%1 %2")
                        .arg("CAN Shark Mini on", serial_port.portName());

                _ret->push_back({name, serial_port.portName()});
            }
        }

        return *_ret;
    }


} // drivers
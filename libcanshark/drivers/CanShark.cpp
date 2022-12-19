#include "CanShark.h"
#include <QSerialPortInfo>
#include <iostream>
#include <RecordingThread.h>

namespace dd::libcanshark::drivers {

    /// Constructors && Destructors
    CanShark::CanShark(QObject *parent) : QObject(parent) {}

    CanShark::~CanShark() {
        delete ptr_recordingThread;
        delete ptr_firmwareUpdateThread;
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



} // drivers
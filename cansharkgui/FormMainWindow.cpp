#include "FormMainWindow.h"
#include "ui_FormMainWindow.h"

#include "LibCanShark.h"
#include <sstream>
#include <iostream>
#include <QFileDialog>
#include <QSerialPortInfo>

#include <iostream>
#include <chrono>
#include <thread>
#include "RecordDisplayItem.h"
#include "RecordItem.h"

namespace dd::forms {
    FormMainWindow::FormMainWindow(QWidget *parent) :
            QWidget(parent), ui(new Ui::FormMainWindow) {
        ui->setupUi(this);

        connect(ui->connectButton, &QPushButton::released, this, &FormMainWindow::connectClicked);
        connect(ui->disconnectButton, &QPushButton::released, this, &FormMainWindow::disconnectClicked);
        connect(ui->startButton, &QPushButton::released, this, &FormMainWindow::startClicked);
        connect(ui->stopButton, &QPushButton::released, this, &FormMainWindow::stopClicked);
        connect(ui->updateDeviceFirmwareButton, &QPushButton::released, this, &FormMainWindow::updateClicked);
        connect(&port, &QSerialPort::readyRead, this, &FormMainWindow::readPort);

        for(const auto& serial_port : QSerialPortInfo::availablePorts()) {
//            std::cout << serial_port.portName().toStdString() << " "
//                      << serial_port.description().toStdString() << " "
//                      << serial_port.manufacturer().toStdString() << " "
//                      << serial_port.serialNumber().toStdString() << " "
//            << std::endl;

            if(serial_port.serialNumber() == "CANSHARKMINI") {
                std::stringstream name_ss;
                name_ss << "CAN Shark Mini on " << serial_port.portName().toStdString();

                this->ui->deviceSelectionComboBox->addItem(QString::fromStdString(name_ss.str()), {serial_port.portName()});
            }
        }
    }

    FormMainWindow::~FormMainWindow() {
        delete ui;
    }

    void FormMainWindow::connectClicked() {
        QString port_name = this->ui->deviceSelectionComboBox->currentData().toString();

        port.setPortName(port_name);
        port.setBaudRate(QSerialPort::Baud115200);
        port.setDataBits(QSerialPort::Data8);
        port.setParity(QSerialPort::NoParity);
        port.setStopBits(QSerialPort::OneStop);
        port.setFlowControl(QSerialPort::NoFlowControl);

        if (port.open(QIODevice::ReadWrite)) {
            this->ui->outputText->appendPlainText("Connected\n");
        } else {
            this->ui->outputText->appendPlainText("Error Connecting\n");

        }
    }

    void FormMainWindow::disconnectClicked() {
        if (port.isOpen()) {
            port.close();

            this->ui->outputText->appendPlainText("Disconnected\n");
        }

    }

    void FormMainWindow::startClicked() {
        if (port.isOpen()) {
            port.write(QByteArray::fromStdString("m"));
        }
    }

    void FormMainWindow::stopClicked() {
        if (port.isOpen()) {
            port.write(QByteArray::fromStdString("n"));
        }
    }

    void FormMainWindow::readPort() {
        const QByteArray in_data = port.readAll();
        port.flush();

        std::vector<std::vector<uint8_t>> packets;
        bool inPacket = false;
        std::vector<uint8_t> tmp_data;

        //This needs to be done better... 
        for(int i = 0, j = 1; i < in_data.size(); i++, j++) {
            if(in_data.at(i) == 0x30 && in_data.at(j) == 0x31)
            {
                i = j + 1;
                j++;
                tmp_data.clear();
                inPacket = true;
                continue;
            }

            if(in_data.at(i) == 0x0d && in_data.at(j) == 0x0a)
            {
                i = 0;
                j = 0;
                inPacket = false;
                packets.emplace_back(tmp_data);
                continue;
            }

            if(inPacket)
                tmp_data.emplace_back(in_data.at(i));

        }

        for(const auto& packetData : packets) {
            data::RecordItem packetItem;

            //TODO(Demi): Deserialize all the data
            memcpy(&packetItem.total_size, packetData.data() + 0, sizeof(uint32_t));

            this->ui->dataContainer->addWidget(new dd::forms::widgets::RecordDisplayItem(packetItem, this));
        }
    }

    void split(const QByteArray & a,
               QList<QByteArray> & l,
               int n)
    {
        for (int i = 0; i < a.size(); i += n)
            l.push_back(a.mid(i, n));
    }

    void FormMainWindow::updateClicked() {
        if(port.isOpen()) {
            auto file_name =
                    QFileDialog::getOpenFileName(
                            this,
                            tr("Select Update"),
                            "/home",
                            tr("Update Files (*.csu)"));

            QFile input(file_name);

            if(!input.open(QFile::OpenModeFlag::ReadOnly))
                return;

            port.write(QByteArray::fromStdString("u")); //Put in update mode

            qint64 update_size = input.size();

            port.write(reinterpret_cast<const char *>(&update_size), sizeof(qint64));

            port.flush();

            //Wait for update mode to kick in
            std::this_thread::sleep_for(std::chrono::milliseconds(100));

            QByteArray file_data = input.readAll();
            QList<QByteArray> lines;

            split(file_data, lines, 512);

            qint64 progress_count = 0;

            for(const auto& line : lines) {
                qint64 written_count = port.write(line);
                port.flush();

                progress_count += written_count;

                std::cout << "Wrote: " << written_count << " bytes" << std::endl;
                std::cout << progress_count << " of " << file_data.size() << std::endl;

                port.waitForReadyRead(100);
                auto data = port.readAll();

                std::cout << "Read: " << data.size() << std::endl;
            }

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
////                if(!port.waitForBytesWritten(100)) {
////                    std::cout << "Error writting bytes" << std::endl;
////                }
//            }

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


//            port.write(file_data);

        }
    }
} // dd::forms

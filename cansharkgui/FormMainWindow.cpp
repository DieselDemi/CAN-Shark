#include "FormMainWindow.h"
#include "ui_FormMainWindow.h"

#include "LibCanShark.h"
#include <sstream>
#include <iostream>
#include <QFileDialog>
#include <QSerialPortInfo>

#include <chrono>
#include <thread>
#include <QSignalMapper>
#include "RecordItem.h"
#include "RecordTableModel.h"

namespace dd::forms {
    FormMainWindow::FormMainWindow(QWidget *parent) :
            QWidget(parent), ui(new Ui::FormMainWindow) {
        ui->setupUi(this);

        connect(ui->connectButton, &QPushButton::released,
                this, &FormMainWindow::connectClicked);

        connect(ui->disconnectButton, &QPushButton::released,
                this, &FormMainWindow::disconnectClicked);

        connect(ui->startButton, &QPushButton::released,
                this, &FormMainWindow::startClicked);

        connect(ui->stopButton, &QPushButton::released,
                this, &FormMainWindow::stopClicked);

        connect(ui->updateDeviceFirmwareButton, &QPushButton::released,
                this, &FormMainWindow::updateClicked);

        connect(ui->saveRecordedDataButton, &QPushButton::released,
                this, &FormMainWindow::saveRecordedDataClicked);

        connect(ui->clearOutputLogButton, &QPushButton::released,
                this, &FormMainWindow::clearLogOutput);


        connect(&canSharkThread, &LibCanShark::error,
                this, &FormMainWindow::serialError);

        connect(&canSharkThread, &LibCanShark::response,
                this, &FormMainWindow::serialResponse);

        connect(&canSharkThread, &LibCanShark::warn,
                this, &FormMainWindow::serialWarn);

        connect(&canSharkThread, &LibCanShark::message,
                this, &FormMainWindow::serialMessage);

        this->ui->disconnectButton->setEnabled(false);
        this->ui->stopButton->setEnabled(false);

        recordTableModelPtr = std::make_unique<models::RecordTableModel>(ui->recordTable);
        this->ui->recordTable->setModel(recordTableModelPtr.get());

        for(const auto& serial_port : QSerialPortInfo::availablePorts()) {
            std::cout << serial_port.portName().toStdString() << " "
                      << serial_port.description().toStdString() << " "
                      << serial_port.manufacturer().toStdString() << " "
                      << serial_port.serialNumber().toStdString() << " "
            << std::endl;

            //TODO: Find the name on linux distros, below is windows and mac
            if(serial_port.serialNumber() == "CANSHARKMINI" || serial_port.portName().contains("cu.SLAB_USBtoUART")) {
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
        canSharkThread.connect(this->ui->deviceSelectionComboBox->currentData().toString(), 1000);
        this->ui->disconnectButton->setEnabled(true);
        this->ui->connectButton->setEnabled(false);
    }

    void FormMainWindow::disconnectClicked() {
        canSharkThread.disconnect();
        this->ui->disconnectButton->setEnabled(false);
        this->ui->connectButton->setEnabled(true);
    }

    void FormMainWindow::startClicked() {
        canSharkThread.startRecording();

        this->ui->startButton->setEnabled(false);
        this->ui->stopButton->setEnabled(true);
    }

    void FormMainWindow::stopClicked() {
        canSharkThread.stopRecording();

        this->ui->startButton->setEnabled(true);
        this->ui->stopButton->setEnabled(false);
    }

    void split(const QByteArray & a,
               QList<QByteArray> & l,
               int n)
    {
        for (int i = 0; i < a.size(); i += n)
            l.push_back(a.mid(i, n));
    }

    void FormMainWindow::updateClicked() {
        return;
//
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
////            while(!input.atEnd()) {
////
////                QByteArray line = input.readLine();
////                qint64 written_count = port.write(line);
////
////                std::cout << line.size() << " == " << written_count << std::endl;
////                assert(line.size() == written_count);
////
////                port.flush();
////
////                while(port.bytesToWrite() != 0){
////                    port.waitForBytesWritten(10);
////                }
////
//////                if(!port.waitForBytesWritten(100)) {
//////                    std::cout << "Error writting bytes" << std::endl;
//////                }
////            }
//
////            for(const auto& byte : file_data.toStdString()) {
////                port.write(reinterpret_cast<const char *>(byte));
////                port.
////                auto read_data = port.readAll();
////
////                for(const auto& read_byte : read_data) {
////                    std::cout << std::hex << read_byte << " ";
////                }
////                std::cout << std::endl;
////            }
//
//
////            port.write(file_data);
//
//        }
    }

    void FormMainWindow::saveRecordedDataClicked() {
        //TODO Implement this
        for(int m = 0; m < 1000; m++) {

            data::RecordItem item = {
                    .total_size = 0,
                    .type = data::CanFrameType::STANDARD,
                    .time = 10000,
                    .id = 100,
                    .data = NULL,
                    .crc16 = 54353
            };

            recordTableModelPtr->addRow(item);

        }
    }

    void FormMainWindow::serialError(const QString &s) {
        this->ui->outputText->appendPlainText("ERROR: ");
        this->ui->outputText->appendPlainText(s);
        this->ui->outputText->appendPlainText("\n");
    }

    void FormMainWindow::serialResponse(const QString &s) {

        return;

        for(char c : s.toStdString()) {
            if(c  == '<') {
                bAppendToPacket = true;
                continue;
            } else if (c == '>') {
                bAppendToPacket = false;
                packetHexStrings.append(packetHexString);
                packetHexString.clear();
            }

            if(bAppendToPacket) {
                packetHexString.append(c);
            }
        }

        if(bAppendToPacket)
            return;

        for (const auto &packet: packetHexStrings) {
            QList<QString> byteStrings;

            assert(packet.size() % 2 == 0);

            for(qsizetype i = 0; i < packet.size(); i += 2){
                QString byteString = packet.mid(i, 2);
                byteStrings.append(byteString);
            }

            if(byteStrings.empty())
                continue;

            QString lengthString;

            lengthString = byteStrings[3] +
                           byteStrings[2] +
                           byteStrings[1] +
                           byteStrings[0];

            bool bConvertedOk = false;

            size_t length = lengthString.toULong(&bConvertedOk, 16);
            assert(bConvertedOk);

            auto dataLen = (qsizetype)(length - sizeof(uint16_t));
            uint8_t packetData[dataLen];

            for(qsizetype i = 0; i < dataLen; i++) {
                bConvertedOk = false;
                packetData[dataLen - i] = (uint8_t)byteStrings[4 + i].toUShort(&bConvertedOk, 16);
                assert(bConvertedOk);
            }

            QString crcString = byteStrings[4 + dataLen + 1] +
                                byteStrings[4 + dataLen];

            bConvertedOk = false;
            uint16_t crc = crcString.toUShort(&bConvertedOk, 16);
            assert(bConvertedOk);

            uint64_t time = 0;
            uint16_t type = 0;
            uint32_t id = 0;
            size_t canDataLen = dataLen - 8 + 2 + 4;
            auto* canData = (uint8_t*)malloc(sizeof(uint8_t) * canDataLen);

            memcpy(&time, packetData, 8);
            memcpy(&type, packetData + 9, 2);
            memcpy(&id, packetData + 11, 4);
            memcpy(&canData, packetData + 15, dataLen);

            //1. Convert hex string to a record item
            data::RecordItem data = {
                .total_size = (uint32_t)dataLen,
                .type = static_cast<data::CanFrameType>(type),
                .time = time,
                .id = id,
                .data = canData,
                .crc16 = crc
            };

            //2. Add a record display item with the packetData from the record item
//            auto* recordDisplayItem = new dd::forms::widgets::RecordDisplayItem(data, this);
//            this->ui->dataContainer->addWidget(recordDisplayItem);

            //3. Remove this hex string from the list
            packetHexString.remove(packet);
        }
    }

    void FormMainWindow::serialWarn(const QString &s) {
        this->ui->outputText->appendPlainText("WARN: ");
        this->ui->outputText->appendPlainText(s);
        this->ui->outputText->appendPlainText("\n");
    }

    void FormMainWindow::clearLogOutput() {
        this->ui->outputText->clear();
    }

    void FormMainWindow::serialMessage(const QString &s) {
        this->ui->outputText->appendPlainText(s);
        this->ui->outputText->appendPlainText("\n");
    }

} // dd::forms

#include "FormMainWindow.h"
#include "ui_FormMainWindow.h"

#include <sstream>
#include <iostream>
#include <QFileDialog>
#include <QSerialPortInfo>

#include <thread>
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

        connect(&canSharkThread, &dd::libcanshark::threads::SerialThread::error,
                this, &FormMainWindow::serialError);

        connect(&canSharkThread, &dd::libcanshark::threads::SerialThread::warn,
                this, &FormMainWindow::serialWarn);

        connect(&canSharkThread, &dd::libcanshark::threads::SerialThread::message,
                this, &FormMainWindow::serialMessage);

        connect(&dataThread, &dd::libcanshark::threads::DataParserThread::dataReady,
                this, &FormMainWindow::parsedDataReady);


        this->ui->disconnectButton->setEnabled(false);
        this->ui->stopButton->setEnabled(false);

        recordTableModelPtr = std::make_unique<models::RecordTableModel>(ui->recordTable);
        this->ui->recordTable->setModel((QAbstractTableModel*)recordTableModelPtr.get());

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
        dataThread.init(canSharkThread);

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
    }

    void FormMainWindow::serialError(const QString &s) {
        this->ui->outputText->appendPlainText("ERROR: ");
        this->ui->outputText->appendPlainText(s);
        this->ui->outputText->appendPlainText("\n");
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

    void FormMainWindow::parsedDataReady(QList<dd::libcanshark::data::RecordItem>& data) {
        for (auto &row: data) {
            recordTableModelPtr->addRow(row);
        }
    }

} // dd::forms

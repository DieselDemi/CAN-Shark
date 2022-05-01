#include "frm_main_window.h"
#include "ui_frm_main_window.h"

namespace dd::forms {
    frm_main_window::frm_main_window(QWidget *parent) :
            QWidget(parent), ui(new Ui::frm_main_window) {
        ui->setupUi(this);

        connect(ui->connectButton, &QPushButton::released, this, &frm_main_window::connectClicked);
        connect(ui->disconnectButton, &QPushButton::released, this, &frm_main_window::disconnectClicked);
        connect(ui->startButton, &QPushButton::released, this, &frm_main_window::startClicked);
        connect(ui->stopButton, &QPushButton::released, this, &frm_main_window::stopClicked);

    }

    frm_main_window::~frm_main_window() {
        delete ui;
    }

    void frm_main_window::connectClicked() {
        port.setPortName("COM4");
        port.setBaudRate(QSerialPort::Baud115200);
        port.setDataBits(QSerialPort::Data8);
        port.setParity(QSerialPort::NoParity);
        port.setStopBits(QSerialPort::OneStop);
        port.setFlowControl(QSerialPort::NoFlowControl);

        if (port.open(QIODevice::ReadWrite))
        {
            //Connected
            this->ui->outputText->appendPlainText("Connected\n");

            while(port.bytesAvailable()) {
                this->ui->outputText->appendPlainText(port.readAll());
            }
        }
        else
        {
            //Open error
            this->ui->outputText->appendPlainText("Error\n");
        }
    }

    void frm_main_window::disconnectClicked() {
        port.close();
    }

    void frm_main_window::startClicked() {
        if(port.isOpen()) {
            port.write(QByteArray::fromStdString("m"));
        }
    }

    void frm_main_window::stopClicked() {
        if(port.isOpen()) {
            port.write(QByteArray::fromStdString("n"));
        }
    }
} // dd::forms

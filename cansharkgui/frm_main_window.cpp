#include "frm_main_window.h"
#include "ui_frm_main_window.h"

#include "LibCanShark.h"

namespace dd::forms {
    frm_main_window::frm_main_window(QWidget *parent) :
            QWidget(parent), ui(new Ui::frm_main_window) {
        ui->setupUi(this);

        connect(ui->connectButton, &QPushButton::released, this, &frm_main_window::connectClicked);
        connect(ui->disconnectButton, &QPushButton::released, this, &frm_main_window::disconnectClicked);
        connect(ui->startButton, &QPushButton::released, this, &frm_main_window::startClicked);
        connect(ui->stopButton, &QPushButton::released, this, &frm_main_window::stopClicked);
        connect(&port, &QSerialPort::readyRead, this, &frm_main_window::readPort);
    }

    frm_main_window::~frm_main_window() {
        delete ui;
    }

    void frm_main_window::connectClicked() {
//        const SettingsDialog::Settings p = m_settings->settings();
        port.setPortName("COM4");
        port.setBaudRate(QSerialPort::Baud115200);
        port.setDataBits(QSerialPort::Data8);
        port.setParity(QSerialPort::NoParity);
        port.setStopBits(QSerialPort::OneStop);
        port.setFlowControl(QSerialPort::NoFlowControl);
        if (port.open(QIODevice::ReadWrite)) {
            this->ui->outputText->appendPlainText("Connected\n");
//            ui->console.setEnabled(true);
//            this->ui->console.setLocalEchoEnabled(true);
//            m_ui->actionConnect->setEnabled(false);
//            m_ui->actionDisconnect->setEnabled(true);
//            m_ui->actionConfigure->setEnabled(false);
//            showStatusMessage(tr("Connected to %1 : %2, %3, %4, %5, %6")
//                                      .arg(p.name).arg(p.stringBaudRate).arg(p.stringDataBits)
//                                      .arg(p.stringParity).arg(p.stringStopBits).arg(p.stringFlowControl));
        } else {
            this->ui->outputText->appendPlainText("Error Connecting\n");

        }
    }

    void frm_main_window::disconnectClicked() {
        if (port.isOpen()) {
            port.close();

            this->ui->outputText->appendPlainText("Disconnected\n");
        }

    }

    void frm_main_window::startClicked() {
        if (port.isOpen()) {
            port.write(QByteArray::fromStdString("m"));
        }
    }

    void frm_main_window::stopClicked() {
        if (port.isOpen()) {
            port.write(QByteArray::fromStdString("n"));
        }
    }

    void frm_main_window::readPort() {
        const QByteArray data = port.readAll();
        std::string out = data.toStdString();
        this->ui->outputText->appendPlainText(QString::fromStdString(out));
        this->ui->outputText->appendPlainText("\n");
    }
} // dd::forms

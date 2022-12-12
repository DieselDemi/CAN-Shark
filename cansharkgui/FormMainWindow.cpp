#include "FormMainWindow.h"
#include "ui_FormMainWindow.h"

#include <sstream>
#include <iostream>
#include <QFileDialog>
#include <QSerialPortInfo>

#include <thread>
#include <QMessageBox>
#include "RecordTableModel.h"

namespace dd::forms {
    /**
     * Basic constructor
     * @param parent
     */
    FormMainWindow::FormMainWindow(QWidget *parent) :
            QWidget(parent),
            ui(new Ui::FormMainWindow),
            m_dataThread(new libcanshark::threads::DataParserThread(this)) {

        ui->setupUi(this);

        //Connect UI events
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

        connect(ui->saveCaptureButton, &QPushButton::released,
                this, &FormMainWindow::saveRecordedDataClicked);

        //Connect the data thread data ready signal
        connect(m_dataThread, &dd::libcanshark::threads::DataParserThread::dataReady,
                this, &FormMainWindow::parsedDataReady);

        this->ui->disconnectButton->setEnabled(false);
        this->ui->stopButton->setEnabled(false);

        m_recordTableModelPtr = std::make_unique<models::RecordTableModel>(ui->recordTable);
        this->ui->recordTable->setModel((QAbstractTableModel *) m_recordTableModelPtr.get());


        // Create a CanShark object
        // TODO: Create the driver based on which device is connected
        m_canShark = new libcanshark::drivers::CanSharkMini(m_dataThread, this);

        assert(m_canShark != nullptr);

        connect(m_canShark, &dd::libcanshark::drivers::CanShark::statusMessage,
                this, &FormMainWindow::canSharkMessage);
        connect(m_canShark, &dd::libcanshark::drivers::CanShark::errorMessage,
                this, &FormMainWindow::canSharkError);

        for(const auto& port : m_canShark->getAvailablePorts()) {
            this->ui->deviceSelectionComboBox->addItem(std::get<0>(port), {std::get<1>(port)});
        }
    }

    /**
     * Basic destructor
     */
    FormMainWindow::~FormMainWindow() {
        assert(m_dataThread != nullptr);
        delete m_canShark;

        if(m_dataThread->isRunning())
            m_dataThread->stop();

        delete ui;
    }

    /**
     * Sets the status statusMessage on the bottom of the UI
     * @param message
     */
    void FormMainWindow::setStatusMessage(const QString &message, QColor color) {
        QPalette labelPalette = this->ui->statusLabel->palette();
        labelPalette.setColor(this->ui->statusLabel->foregroundRole(), color);
        this->ui->statusLabel->setPalette(labelPalette);
        this->ui->statusLabel->setText(message);
    }

    //// PRIVATE SLOTS

    /**
     * Called when the user clicks connect
     */
    void FormMainWindow::connectClicked() {
        assert(m_canShark != nullptr);

#ifdef WIN32
        if(!m_canShark->openConnection(tr("%1").arg(this->ui->deviceSelectionComboBox->currentData().toString())))
            QMessageBox::critical(this, tr("Could not connect!"), tr("Could not connect to canshark mini on %1").arg(this->ui->deviceSelectionComboBox->currentData().toString()));
#else
        if(!m_canShark->openConnection(tr("/dev/%1").arg(this->ui->deviceSelectionComboBox->currentData().toString())))
            QMessageBox::critical(this, tr("Could not connect!"), tr("Could not connect to canshark mini on /dev/%1").arg(this->ui->deviceSelectionComboBox->currentData().toString()));
#endif

        this->ui->connectButton->setEnabled(false);
        this->ui->disconnectButton->setEnabled(true);
    }

    /**
     * Called when the user clicks disconnect
     */
    void FormMainWindow::disconnectClicked() {
        if(!m_canShark->closeConnection())
            QMessageBox::critical(this, tr("Could not disconnect!"), tr("Could not disconnect from target!"));

        this->ui->connectButton->setEnabled(true);
        this->ui->disconnectButton->setEnabled(false);
    }

    /**
     * Called when the users clicks start recording
     */
    void FormMainWindow::startClicked() {
        if(!m_canShark->startRecording(0))
            return;

        this->ui->startButton->setEnabled(false);
        this->ui->stopButton->setEnabled(true);
    }

    /**
     * Called when the user clicks stop recording
     */
    void FormMainWindow::stopClicked() {
        if(!m_canShark->stopRecording())
            return;

        this->ui->startButton->setEnabled(true);
        this->ui->stopButton->setEnabled(false);
    }

    /**
     * Called when the user clicks update
     */
    void FormMainWindow::updateClicked() {
        //TODO Implement this method
        m_canShark->updateFirmware("");
    }

    /**
     * Called when the user clicks save recorded data
     */
    void FormMainWindow::saveRecordedDataClicked() {
        //TODO Implement this
    }

    /**
     * Called when the DataParserThread has data ready to be added
     * @param data
     */
    void FormMainWindow::parsedDataReady(QList<dd::libcanshark::data::RecordItem> &data) {
        for (auto &row: data) {
            m_recordTableModelPtr->addRow(row);
        }
    }

    void FormMainWindow::canSharkMessage(const QString &message) {
        setStatusMessage(message);
    }

    void FormMainWindow::canSharkError(const QString &message) {
        setStatusMessage(message, Qt::red);
    }

} // dd::forms

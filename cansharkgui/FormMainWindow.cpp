#include "FormMainWindow.h"
#include "ui_FormMainWindow.h"

#include <QFileDialog>
#include <QSerialPortInfo>

#include <thread>
#include <QMessageBox>
#include "RecordTableModel.h"
#include "FormSettings.h"

namespace dd::forms {
    /**
     * Basic constructor
     * @param parent
     */
    FormMainWindow::FormMainWindow(QApplication *app, QWidget *parent) :
            QWidget(parent), ui(new Ui::FormMainWindow), ptr_mainApplication(app) {

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

        connect(ui->settingsButton, &QPushButton::released,
                this, &FormMainWindow::settingsButtonClicked);

        connect(ui->defaultRadioButton, &QRadioButton::clicked, this, &FormMainWindow::defaultRadioButtonClicked);
        connect(ui->onlyShowUniqueRadioButton, &QRadioButton::clicked, this,
                &FormMainWindow::onlyShowUniqueRadioButtonClicked);

        this->ui->disconnectButton->setEnabled(false);
        this->ui->stopButton->setEnabled(false);

        this->ui->defaultRadioButton->setChecked(true);

        m_recordTableModelPtr = new models::RecordTableModel(ui->recordTable);
        this->ui->recordTable->setModel((QAbstractTableModel *) m_recordTableModelPtr);


        // Create a CanShark object
        // TODO: Create the driver based on which device is connected
        m_driverCanShark = new libcanshark::drivers::CanSharkMini();

        assert(ptr_driverCanShark != nullptr);

        connect(m_driverCanShark, &dd::libcanshark::drivers::CanShark::statusMessage,
                this, &FormMainWindow::canSharkMessage);
        connect(m_driverCanShark, &dd::libcanshark::drivers::CanShark::errorMessage,
                this, &FormMainWindow::canSharkError);

        connect(m_driverCanShark, &dd::libcanshark::drivers::CanShark::updateComplete,
                this, &FormMainWindow::canSharkUpdateComplete);

        for (const auto &port: m_driverCanShark->getAvailablePorts()) {
            this->ui->deviceSelectionComboBox->addItem(std::get<0>(port), {std::get<1>(port)});
        }
    }

    /**
     * Basic destructor
     */
    FormMainWindow::~FormMainWindow() {
        assert(ptr_dataThread != nullptr);

        delete ui;
    }

    /**
     * Sets the status progressMessage on the bottom of the UI
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
        assert(ptr_driverCanShark != nullptr);

#ifdef WIN32
        if(!m_driverCanShark->openConnection(tr("%1").arg(this->ui->deviceSelectionComboBox->currentData().toString())))
            QMessageBox::critical(this, tr("Could not connect!"), tr("Could not connect to canshark mini on %1").arg(this->ui->deviceSelectionComboBox->currentData().toString()));
#else
        if (!m_driverCanShark->openConnection(
                tr("/dev/%1").arg(this->ui->deviceSelectionComboBox->currentData().toString())))
            QMessageBox::critical(this, tr("Could not connect!"),
                                  tr("Could not connect to canshark mini on /dev/%1").arg(
                                          this->ui->deviceSelectionComboBox->currentData().toString()));
#endif

        this->ui->connectButton->setEnabled(false);
        this->ui->disconnectButton->setEnabled(true);

        //Connect the data thread data ready signal
        connect(this->m_driverCanShark->getDataParserThread(), &dd::libcanshark::threads::DataParserThread::dataReady,
                this, &FormMainWindow::parsedDataReady);
    }

    /**
     * Called when the user clicks disconnect
     */
    void FormMainWindow::disconnectClicked() {
        if (!m_driverCanShark->closeConnection())
            QMessageBox::critical(this, tr("Could not disconnect!"), tr("Could not disconnect from target!"));

        this->ui->connectButton->setEnabled(true);
        this->ui->disconnectButton->setEnabled(false);

        disconnect(this->m_driverCanShark->getDataParserThread(), &dd::libcanshark::threads::DataParserThread::dataReady,
                this, &FormMainWindow::parsedDataReady);
    }

    /**
     * Called when the users clicks start recording
     */
    void FormMainWindow::startClicked() {
        if (!m_driverCanShark->startRecording(0))
            return;

        this->ui->startButton->setEnabled(false);
        this->ui->stopButton->setEnabled(true);
    }

    /**
     * Called when the user clicks stop recording
     */
    void FormMainWindow::stopClicked() {
        if (!m_driverCanShark->stopRecording())
            return;

        this->ui->startButton->setEnabled(true);
        this->ui->stopButton->setEnabled(false);
    }

    /**
     * Called when the user clicks update
     */
    void FormMainWindow::updateClicked() {
        auto fileName = QFileDialog::getOpenFileName(this,
                tr("Open Firmware Update"), "/home/", tr("Firmware Update Files (*.cfu)"));

        if(fileName.isEmpty())
            return;

        if(m_driverCanShark->updateFirmware(fileName))
            this->setEnabled(false);
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
        if(data.empty())
            return;

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

    void FormMainWindow::defaultRadioButtonClicked(bool checked) {
        this->ui->onlyShowUniqueRadioButton->setChecked(!checked);
    }

    void FormMainWindow::onlyShowUniqueRadioButtonClicked(bool checked) {
        this->ui->defaultRadioButton->setChecked(!checked);
    }

    void FormMainWindow::settingsButtonClicked() {
        if (m_formSettings == nullptr) {
            assert(this->ptr_mainApplication != nullptr);
            assert(this->m_driverCanShark != nullptr);
            this->m_formSettings = new FormSettings(this->ptr_mainApplication, this->m_driverCanShark);
            this->m_formSettings->show();
        } else {
            this->m_formSettings->show();
        }
    }

    void FormMainWindow::canSharkUpdateComplete(dd::libcanshark::threads::FirmwareUpdateThreadStatus status) {
        switch(status) {
            case libcanshark::threads::FirmwareUpdateThreadStatus::Success:
                QMessageBox::information(this, tr("Update Complete"), tr("Firmware has been updated successfully"));
                break;
            case libcanshark::threads::FirmwareUpdateThreadStatus::Fail:
                QMessageBox::critical(this, tr("Firmware Update has Failed"), tr("Something went wrong with the firmware update!"));
                break;
        }
        this->setEnabled(true);
    }

} // dd::forms

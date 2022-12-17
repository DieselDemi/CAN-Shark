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
            QWidget(parent),
            ui(new Ui::FormMainWindow),
            ptr_dataThread(new libcanshark::threads::DataParserThread(this)),
            ptr_mainApplication(app) {

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

        //Connect the data thread data ready signal
        connect(ptr_dataThread, &dd::libcanshark::threads::DataParserThread::dataReady,
                this, &FormMainWindow::parsedDataReady);

        this->ui->disconnectButton->setEnabled(false);
        this->ui->stopButton->setEnabled(false);

        this->ui->defaultRadioButton->setChecked(true);

        m_recordTableModelPtr = std::make_unique<models::RecordTableModel>(ui->recordTable);
        this->ui->recordTable->setModel((QAbstractTableModel *) m_recordTableModelPtr.get());


        // Create a CanShark object
        // TODO: Create the driver based on which device is connected
        ptr_driverCanShark = new libcanshark::drivers::CanSharkMini(ptr_dataThread, this);

        assert(ptr_driverCanShark != nullptr);

        connect(ptr_driverCanShark, &dd::libcanshark::drivers::CanShark::statusMessage,
                this, &FormMainWindow::canSharkMessage);
        connect(ptr_driverCanShark, &dd::libcanshark::drivers::CanShark::errorMessage,
                this, &FormMainWindow::canSharkError);

        for (const auto &port: ptr_driverCanShark->getAvailablePorts()) {
            this->ui->deviceSelectionComboBox->addItem(std::get<0>(port), {std::get<1>(port)});
        }
    }

    /**
     * Basic destructor
     */
    FormMainWindow::~FormMainWindow() {
        assert(ptr_dataThread != nullptr);
        delete ptr_driverCanShark;

        if (ptr_dataThread->isRunning())
            ptr_dataThread->stop();

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
        assert(ptr_driverCanShark != nullptr);

#ifdef WIN32
        if(!ptr_driverCanShark->openConnection(tr("%1").arg(this->ui->deviceSelectionComboBox->currentData().toString())))
            QMessageBox::critical(this, tr("Could not connect!"), tr("Could not connect to canshark mini on %1").arg(this->ui->deviceSelectionComboBox->currentData().toString()));
#else
        if (!ptr_driverCanShark->openConnection(
                tr("/dev/%1").arg(this->ui->deviceSelectionComboBox->currentData().toString())))
            QMessageBox::critical(this, tr("Could not connect!"),
                                  tr("Could not connect to canshark mini on /dev/%1").arg(
                                          this->ui->deviceSelectionComboBox->currentData().toString()));
#endif

        this->ui->connectButton->setEnabled(false);
        this->ui->disconnectButton->setEnabled(true);
    }

    /**
     * Called when the user clicks disconnect
     */
    void FormMainWindow::disconnectClicked() {
        if (!ptr_driverCanShark->closeConnection())
            QMessageBox::critical(this, tr("Could not disconnect!"), tr("Could not disconnect from target!"));

        this->ui->connectButton->setEnabled(true);
        this->ui->disconnectButton->setEnabled(false);
    }

    /**
     * Called when the users clicks start recording
     */
    void FormMainWindow::startClicked() {
        if (!ptr_driverCanShark->startRecording(0))
            return;

        this->ui->startButton->setEnabled(false);
        this->ui->stopButton->setEnabled(true);
    }

    /**
     * Called when the user clicks stop recording
     */
    void FormMainWindow::stopClicked() {
        if (!ptr_driverCanShark->stopRecording())
            return;

        this->ui->startButton->setEnabled(true);
        this->ui->stopButton->setEnabled(false);
    }

    /**
     * Called when the user clicks update
     */
    void FormMainWindow::updateClicked() {
        //TODO Implement this method
        ptr_driverCanShark->updateFirmware("");
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

    void FormMainWindow::defaultRadioButtonClicked(bool checked) {
        this->ui->onlyShowUniqueRadioButton->setChecked(!checked);
    }

    void FormMainWindow::onlyShowUniqueRadioButtonClicked(bool checked) {
        this->ui->defaultRadioButton->setChecked(!checked);
    }

    void FormMainWindow::settingsButtonClicked() {
        if (ptr_formSettings == nullptr) {
            assert(this->ptr_mainApplication != nullptr);
            assert(this->ptr_driverCanShark != nullptr);
            this->ptr_formSettings = new FormSettings(this->ptr_mainApplication, this->ptr_driverCanShark);
            this->ptr_formSettings->show();
        } else {
            this->ptr_formSettings->show();
        }
    }

} // dd::forms

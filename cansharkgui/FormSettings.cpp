#include "FormSettings.h"
#include "ui_FormSettings.h"
#include "BaseTheme.h"

namespace dd::forms {
    FormSettings::FormSettings(QApplication* app, dd::libcanshark::drivers::CanShark* canShark, QWidget *parent) :
            QWidget(parent), ui(new Ui::FormSettings), ptr_mainApplication(app), ptr_driverCanShark(canShark) {
        ui->setupUi(this);

        //Base ui
        connect(ui->okButton, &QPushButton::released, this, &FormSettings::okButtonClicked);

        //General tab
        connect(ui->themeComboBox, &QComboBox::currentIndexChanged, this, &FormSettings::themeComboBoxIndexChanged);

        //Add available themes to the combo box
        for(auto Theme : theme::Themes) {
            this->ui->themeComboBox->addItem(Theme.name);
        }
        this->ui->themeComboBox->setCurrentIndex((int)theme::ThemeType::Darcula);

        //Device tab
        connect(ui->saveToDeviceButton, &QPushButton::released, this, &FormSettings::saveToDeviceButtonClicked);
        connect(ui->deviceComboBox, &QComboBox::currentIndexChanged, this, &FormSettings::deviceComboBoxIndexChanged);

        //Give the driver section all the available devices
        for(const auto& shark : this->ptr_driverCanShark->getAvailablePorts()) {
            this->ui->deviceComboBox->addItem(std::get<0>(shark), std::get<1>(shark));
        }
        this->ui->baudRateComboBox->setCurrentIndex(5);
    }

    FormSettings::~FormSettings() {
        delete ui;
    }

    void FormSettings::okButtonClicked() {
        this->close();
    }

    void FormSettings::themeComboBoxIndexChanged(int idx) {
        this->ptr_mainApplication->setStyleSheet(theme::Themes[idx].theme);
    }

    void FormSettings::deviceComboBoxIndexChanged(int idx) {
        this->selectedDevicePort = this->ui->deviceComboBox->itemData(idx).toString();

        updateDeviceLabelText();
    }

    void FormSettings::saveToDeviceButtonClicked() {
        //TODO: Connect to currently selected device, send settings, then close connection
        this->ptr_driverCanShark->openConnection(this->selectedDevicePort);
//        this->ptr_driverCanShark->sendSettings();
        this->ptr_driverCanShark->closeConnection();
    }

    void FormSettings::updateDeviceLabelText() {
        //TODO: Connect to currently selected device, get device info, close the connection
        this->ptr_driverCanShark->openConnection(this->selectedDevicePort);
//        this->ptr_driverCanShark->getDeviceInfo();
        this->ptr_driverCanShark->closeConnection();
    }

} // dd::forms

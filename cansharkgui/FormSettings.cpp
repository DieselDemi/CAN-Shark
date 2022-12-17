#include "FormSettings.h"
#include "ui_FormSettings.h"
#include "BaseTheme.h"

namespace dd::forms {
    FormSettings::FormSettings(QApplication* app, QWidget *parent) :
            QWidget(parent), ui(new Ui::FormSettings), ptr_mainApplication(app) {
        ui->setupUi(this);

        connect(ui->okButton, &QPushButton::released, this, &FormSettings::okButtonClicked);
        connect(ui->cancelButton, &QPushButton::released, this, &FormSettings::cancelButtonClicked);
        connect(ui->themeComboBox, &QComboBox::currentIndexChanged, this, &FormSettings::themeComboBoxIndexChanged);
        connect(ui->connectButton, &QPushButton::released, this, &FormSettings::connectButtonClicked);
        connect(ui->disconnectButton, &QPushButton::released, this, &FormSettings::disconnectButtonClicked);
        connect(ui->saveToDeviceButton, &QPushButton::released, this, &FormSettings::saveToDeviceButtonClicked);

        //Add available themes to the combo box
        for(auto Theme : theme::Themes) {
            this->ui->themeComboBox->addItem(Theme.name);
        }

        this->ui->themeComboBox->setCurrentIndex(2);
    }

    FormSettings::~FormSettings() {
        delete ui;
    }

    void FormSettings::okButtonClicked() {

    }

    void FormSettings::cancelButtonClicked() {
        this->close();
    }

    void FormSettings::themeComboBoxIndexChanged(int idx) {
        this->ptr_mainApplication->setStyleSheet(theme::Themes[idx].theme);
    }

    void FormSettings::connectButtonClicked() {

    }

    void FormSettings::disconnectButtonClicked() {

    }

    void FormSettings::saveToDeviceButtonClicked() {

    }
} // dd::forms

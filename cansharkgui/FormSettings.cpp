#include "FormSettings.h"
#include "ui_FormSettings.h"

namespace dd::forms {
    FormSettings::FormSettings(QWidget *parent) :
            QWidget(parent), ui(new Ui::FormSettings) {
        ui->setupUi(this);
    }

    FormSettings::~FormSettings() {
        delete ui;
    }
} // dd::forms

//
// Created by Demi on 2022-12-16.
//

// You may need to build the project (run Qt uic code generator) to get "ui_FormSettings.h" resolved

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

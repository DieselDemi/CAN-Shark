//
// Created by Demi on 2022-12-16.
//

#ifndef CANSHARK_FORMSETTINGS_H
#define CANSHARK_FORMSETTINGS_H

#include <QWidget>

namespace dd::forms {
    QT_BEGIN_NAMESPACE
    namespace Ui { class FormSettings; }
    QT_END_NAMESPACE

    class FormSettings : public QWidget {
    Q_OBJECT

    public:
        explicit FormSettings(QWidget *parent = nullptr);

        ~FormSettings() override;

    private:
        Ui::FormSettings *ui;
    };
} // dd::forms

#endif //CANSHARK_FORMSETTINGS_H

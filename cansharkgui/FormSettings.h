#ifndef CANSHARK_FORMSETTINGS_H
#define CANSHARK_FORMSETTINGS_H

#include <QWidget>
#include "drivers/CanShark.h"

namespace dd::forms {
    QT_BEGIN_NAMESPACE
    namespace Ui { class FormSettings; }
    QT_END_NAMESPACE

    class FormSettings : public QWidget {
    Q_OBJECT

    public:
        explicit FormSettings(QApplication* app, dd::libcanshark::drivers::CanShark* canShark, QWidget *parent = nullptr);

        ~FormSettings() override;

    private:
        Ui::FormSettings *ui;
        QApplication* ptr_mainApplication = nullptr;

        dd::libcanshark::drivers::CanShark* ptr_driverCanShark = nullptr;

        QString selectedDevicePort;

        void updateDeviceLabelText();

    private slots:
        void okButtonClicked();

        void themeComboBoxIndexChanged(int idx);
        void deviceComboBoxIndexChanged(int idx);

        void saveToDeviceButtonClicked();
    };
} // dd::forms

#endif //CANSHARK_FORMSETTINGS_H

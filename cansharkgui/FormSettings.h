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
        explicit FormSettings(QApplication* app, QWidget *parent = nullptr);

        ~FormSettings() override;

    private:
        Ui::FormSettings *ui;
        QApplication* ptr_mainApplication = nullptr;

    private slots:
        void okButtonClicked();
        void cancelButtonClicked();

        void themeComboBoxIndexChanged(int idx);
        void connectButtonClicked();
        void disconnectButtonClicked();

        void saveToDeviceButtonClicked();
    };
} // dd::forms

#endif //CANSHARK_FORMSETTINGS_H

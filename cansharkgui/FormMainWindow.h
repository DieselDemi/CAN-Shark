#ifndef CAN_SHARK_MINI_FRM_MAIN_WINDOW_H
#define CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include "RecordTableModel.h"
#include "threads/DataParserThread.h"
#include "drivers/CanSharkDrivers.h"
#include "FormSettings.h"

namespace dd::forms {
    QT_BEGIN_NAMESPACE
    namespace Ui { class FormMainWindow; }
    QT_END_NAMESPACE

    class FormMainWindow : public QWidget {
    Q_OBJECT

    public:
        explicit FormMainWindow(QApplication* app, QWidget *parent = nullptr);

        ~FormMainWindow() override;

    private:
        QApplication* ptr_mainApplication = nullptr;
        Ui::FormMainWindow *ui;

        dd::libcanshark::drivers::CanShark* m_driverCanShark = nullptr;
        models::RecordTableModel* m_recordTableModelPtr = nullptr;
        FormSettings* m_formSettings = nullptr;

        QString m_selectedDevicePortName;

        void setStatusMessage(const QString &message, QColor color = Qt::white);

    private slots:
        void deviceSelectionChanged(int index);
        void connectClicked();
        void disconnectClicked();
        void startClicked();
        void stopClicked();
        void updateClicked();
        void saveRecordedDataClicked();
        void settingsButtonClicked();

        void defaultRadioButtonClicked(bool checked);
        void onlyShowUniqueRadioButtonClicked(bool checked);

        void canSharkMessage(QString const& message);
        void canSharkError(QString const& message);
        void canSharkUpdateComplete(dd::libcanshark::threads::FirmwareUpdateThreadStatus status);

        void parsedDataReady(QList<dd::libcanshark::data::RecordItem>& data);
    };
} // dd::forms
#endif //CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

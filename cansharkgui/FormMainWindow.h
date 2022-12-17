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

        dd::libcanshark::threads::DataParserThread* ptr_dataThread = nullptr;

        dd::libcanshark::drivers::CanShark* ptr_driverCanShark = nullptr;

        std::unique_ptr<models::RecordTableModel> m_recordTableModelPtr;

        FormSettings* ptr_formSettings = nullptr;

        void setStatusMessage(const QString &message, QColor color = Qt::white);

    private slots:
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

        void parsedDataReady(QList<dd::libcanshark::data::RecordItem>& data);
    };
} // dd::forms
#endif //CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

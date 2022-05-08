#ifndef CAN_SHARK_MINI_FRM_MAIN_WINDOW_H
#define CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include "LibCanShark.h"

namespace dd::forms {
    QT_BEGIN_NAMESPACE
    namespace Ui { class FormMainWindow; }
    QT_END_NAMESPACE

    class FormMainWindow : public QWidget {
    Q_OBJECT

    public:
        explicit FormMainWindow(QWidget *parent = nullptr);

        ~FormMainWindow() override;

    private:
//        LibCanShark canshark_thread;
        Ui::FormMainWindow *ui;
        QSerialPort port;

    private slots:
        void connectClicked();
        void disconnectClicked();
        void startClicked();
        void stopClicked();
        void updateClicked();

        void readPort();
    };
} // dd::forms

#endif //CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

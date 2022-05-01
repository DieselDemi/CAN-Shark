#ifndef CAN_SHARK_MINI_FRM_MAIN_WINDOW_H
#define CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

#include <QWidget>
#include <QtSerialPort/QSerialPort>
#include "LibCanShark.h"

namespace dd::forms {
    QT_BEGIN_NAMESPACE
    namespace Ui { class frm_main_window; }
    QT_END_NAMESPACE

    class frm_main_window : public QWidget {
    Q_OBJECT

    public:
        explicit frm_main_window(QWidget *parent = nullptr);

        ~frm_main_window() override;

    private:
//        LibCanShark canshark_thread;
        Ui::frm_main_window *ui;
        QSerialPort port;

    private slots:
        void connectClicked();
        void disconnectClicked();
        void startClicked();
        void stopClicked();

        void readPort();
    };
} // dd::forms

#endif //CAN_SHARK_MINI_FRM_MAIN_WINDOW_H

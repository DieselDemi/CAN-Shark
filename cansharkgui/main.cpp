#include <QApplication>
#include <QPushButton>

#include "frm_main_window.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    dd::forms::frm_main_window window;
//    xdfCreator.setStyleSheet(dd::forms::style::style);
    window.show();

//    QPushButton button("Hello world!", nullptr);
//    button.resize(200, 100);
//    button.show();



    return QApplication::exec();
}

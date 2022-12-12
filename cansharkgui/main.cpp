#include <QApplication>

#include "FormMainWindow.h"
#include "BaseTheme.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    dd::forms::FormMainWindow window;
    window.setStyleSheet(dd::forms::theme::Dracula2);
    window.show();

    return QApplication::exec();
}

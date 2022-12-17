#include <QApplication>

#include "FormMainWindow.h"
#include "BaseTheme.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    dd::forms::FormMainWindow window(&a);
    a.setStyleSheet(dd::forms::theme::Themes[(int)dd::forms::theme::ThemeType::Darcula].theme);
    window.show();

    return QApplication::exec();
}

#include <QApplication>
#include <QTextStream>
#include "webloginwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    WebLoginWindow window;
    QObject::connect(&window, &QDialog::accepted, [&window]() {
        QTextStream stream(stdout);
        stream << "Log-in finished!";
    });
    window.show();
    return app.exec();
}
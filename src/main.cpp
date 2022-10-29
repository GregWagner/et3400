#include "windows/mainwindow.h"
#include <QApplication>
#include <QDebug>

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);

    MainWindow window;

    window.setWindowTitle("ET-3400 Emulator");
    window.show();

    return app.exec();
}
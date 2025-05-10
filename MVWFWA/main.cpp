#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // Create and show the MainWindow
    MainWindow w;
    w.show();

    return a.exec();
}
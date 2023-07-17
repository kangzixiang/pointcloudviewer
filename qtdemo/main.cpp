
#include <QtWidgets/QApplication>

#include "main_window.h"
#include "openGLWidget.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    openGLWidget p;
    p.setWindowTitle("Point Cloud Viewer");
    w.show();
    p.show();
    return a.exec();
}
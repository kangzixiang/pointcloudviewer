

#include <QtWidgets/QApplication>


#include <fstream>
#include <sstream>
#include <cstring>
#include <vector>

#include "openGLWidget.h"

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    openGLWidget w;
    w.setWindowTitle("Point Cloud Viewer");
    w.show();

    return a.exec();
}

#include <QtWidgets/QApplication>

#include "main_window.h"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  MainWindow w;
  w.setWindowTitle("Point Cloud Viewer");
  w.show();
  return a.exec();
}
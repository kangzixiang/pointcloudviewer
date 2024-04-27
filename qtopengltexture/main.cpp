
#include <QtGui/QGuiApplication>

#include "window.h"

int main(int argc, char *argv[]) {
  QGuiApplication a(argc, argv);
  Window win;
  win.show();
  return a.exec();
}

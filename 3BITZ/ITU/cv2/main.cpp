#include <QApplication>
#include "MainForm.h"

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);
  MainForm mainForm;
  mainForm.show();
  return app.exec();
}

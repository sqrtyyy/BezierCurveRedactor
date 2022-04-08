#include <QApplication>

#include "MainWindow.h"

int main(int argc, char *argv[])
{
  QApplication app(argc, argv);
  auto* window = new MainWindow();
  window->resize(800, 600);
  window->setWindowTitle
    (QApplication::translate("bezier", "Bezier Curve redactor"));

  window->show();

  return QApplication::exec();
}
#include "danmakuclient.h"
#include <QApplication>
#include <QScopedPointer>

int main(int argc, char *argv[]) {
  QScopedPointer<QCoreApplication> a(new QApplication(argc, argv));
  DanmakuClient c;
  c.listen(23527503);
  return a->exec();
}

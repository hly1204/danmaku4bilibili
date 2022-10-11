#include "mainwindow.h"
#include <QApplication>
#include <QScopedPointer>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QScopedPointer<QCoreApplication> a(new QApplication(argc, argv));
    QTranslator                      t;
    if (t.load("danmaku_zh_CN", "i18n")) a->installTranslator(&t);
    MainWindow w;
    w.show();
    return a->exec();
}

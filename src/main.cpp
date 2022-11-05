#include "mainwindow.h"
#include <QApplication>
#include <QScopedPointer>
#include <QTranslator>

int main(int argc, char *argv[])
{
    using namespace Qt::Literals::StringLiterals;
    QScopedPointer<QCoreApplication> a(new QApplication(argc, argv));
    QTranslator                      t;
    if (t.load("danmaku_zh_CN"_L1, a->applicationDirPath() + "/i18n"_L1)) a->installTranslator(&t);
    MainWindow w;
    w.show();
    return a->exec();
}

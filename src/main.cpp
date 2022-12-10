#include "mainwindow.h"
#include <QApplication>
#include <QSettings>
#include <QTranslator>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char **argv)
{
    QApplication::setStyle("Fusion"_L1);
    QApplication a(argc, argv);
    QSettings::setPath(QSettings::IniFormat, QSettings::UserScope, a.applicationDirPath());
    QTranslator t;
    if (t.load(":/translations/qtbase_zh_CN"_L1)) a.installTranslator(&t);
    MainWindow w;
    w.show();
    return a.exec();
}

#include "mainwindow.h"
#include <QApplication>
#include <QScopedPointer>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QScopedPointer a(new QApplication(argc, argv));
    a->setStyle("Fusion"_L1);
    MainWindow w;
    w.show();
    return a->exec();
}

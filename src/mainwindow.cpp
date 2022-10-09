#include "mainwindow.h"
#include "danmakuclient.h"
#include <QInputDialog>
#include <QMenuBar>
#include <QStatusBar>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), centralWidget_(new QWidget(this)), menuBar_(new QMenuBar(this)),
      statusBar_(new QStatusBar(this)), danmakuClient_(new DanmakuClient(this)) {
  setCentralWidget(centralWidget_);
  setMenuBar(menuBar_);
  setStatusBar(statusBar_);
  setWindowTitle(MainWindow::tr("Danmaku Client"));
  setMinimumSize(320, 240);
  resize(320, 240);

  QMenu *menuConfig = menuBar_->addMenu(QMenu::tr("Config"));
  QAction *actionListen = menuConfig->addAction(QAction::tr("Listen"));
  connect(actionListen, &QAction::triggered, this, [this]() {
    bool ok;
    int roomid = QInputDialog::getInt(this, QInputDialog::tr("Please input the room ID"), QInputDialog::tr("room ID"),
                                      0, 1, 2147483647, 1, &ok);
    if (ok) {
      danmakuClient_->listen(roomid);
    }
  });
}

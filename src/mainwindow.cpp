#include "mainwindow.h"
#include "danmakutablemodel.h"
#include <QInputDialog>
#include <QMenuBar>
#include <QResizeEvent>
#include <QStatusBar>
#include <QTableView>
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), centralWidget_(new QWidget(this)), menuBar_(new QMenuBar(this)),
      statusBar_(new QStatusBar(this)), danmakuTableView_(new QTableView(centralWidget_)) {
  setCentralWidget(centralWidget_);
  setMenuBar(menuBar_);
  setStatusBar(statusBar_);
  setWindowTitle(MainWindow::tr("Danmaku Client"));
  setMinimumSize(320, 240);
  danmakuTableView_->setModel(new DanmakuTableModel(this));
  danmakuTableView_->move(2, 2);
  resize(640, 480);

  QMenu *menuConfig = menuBar_->addMenu(QMenu::tr("Config"));
  QAction *actionListen = menuConfig->addAction(QAction::tr("Listen"));
  actionListen->setEnabled(true);
  connect(actionListen, &QAction::triggered, this, [this, actionListen]() {
    bool ok;
    int roomid = QInputDialog::getInt(this, QInputDialog::tr("Please input the room ID"), QInputDialog::tr("room ID"),
                                      0, 1, 2147483647, 1, &ok);
    if (ok) {
      actionListen->setEnabled(false);
      qobject_cast<DanmakuTableModel *>(danmakuTableView_->model())->listen(roomid);
    }
  });
}

void MainWindow::resizeEvent(QResizeEvent *event) {
  danmakuTableView_->resize(centralWidget_->width() - 5, centralWidget_->height() - 5);
  return QMainWindow::resizeEvent(event);
}

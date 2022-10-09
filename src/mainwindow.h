#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class QWidget;
class QMenuBar;
class QStatusBar;
class DanmakuClient;

class MainWindow : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(MainWindow)

public:
  explicit MainWindow(QWidget *parent = nullptr);

private:
  QWidget *centralWidget_;
  QMenuBar *menuBar_;
  QStatusBar *statusBar_;
  DanmakuClient *danmakuClient_;
};

#endif

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

class QWidget;
class QMenuBar;
class QStatusBar;
class QTableView;

class MainWindow : public QMainWindow {
  Q_OBJECT
  Q_DISABLE_COPY_MOVE(MainWindow)

public:
  explicit MainWindow(QWidget *parent = nullptr);

protected:
  void resizeEvent(QResizeEvent *event) override;

private:
  QWidget *centralWidget_;
  QMenuBar *menuBar_;
  QStatusBar *statusBar_;
  QTableView *danmakuTableView_;
};

#endif

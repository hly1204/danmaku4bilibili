#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

// QT
QT_FORWARD_DECLARE_CLASS(QTabWidget);
QT_FORWARD_DECLARE_CLASS(QTableView);
QT_FORWARD_DECLARE_CLASS(QSettings);

class MainWindow : public QMainWindow
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindow)

public:
    explicit MainWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    QSettings  *settings_;
    QTabWidget *tabWidget_;
    QTableView *danmakuTableView_;
};

#endif

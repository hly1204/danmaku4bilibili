#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

// QT
class QTabWidget;
class QTableView;
class QSettings;

// CUSTOM
class DanmakuClient;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected:
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    Q_DISABLE_COPY_MOVE(MainWindow)

private:
    DanmakuClient *danmakuClient_;
    QSettings     *settings_;
    QTabWidget    *tabWidget_;
    QTableView    *danmakuTableView_;
    QTableView    *giftTableView_;
};

#endif

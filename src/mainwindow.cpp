#include "mainwindow.h"
#include "danmakuclient.h"
#include "danmakutablemodel.h"
#include <QApplication>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenuBar>
#include <QResizeEvent>
#include <QStatusBar>
#include <QString>
#include <QTableView>
#include <QTimer>
#include <QWidget>
#include <numeric>

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      danmakuTableView_(new QTableView)
{
    setCentralWidget(new QWidget(this));
    setMenuBar(new QMenuBar(this));
    setStatusBar(new QStatusBar(this));
    setWindowTitle(u"弹幕客户端"_s);
    setMinimumSize(320, 240);
    auto danmakuClient = new DanmakuClient(this);
    danmakuTableView_->setParent(centralWidget());
    {
        auto danmakuTableModel = new DanmakuTableModel(this);
        danmakuTableModel->setDanmakuClient(danmakuClient);
        danmakuTableView_->setModel(danmakuTableModel);
    }
    danmakuTableView_->move(2, 2);
    resize(640, 480);
    // 配置 menuBar
    {
        QMenu   *menuConfig   = menuBar()->addMenu(u"配置"_s);
        QAction *actionListen = menuConfig->addAction(u"监听"_s);
        QAction *actionStop   = menuConfig->addAction(u"停止"_s);
        actionListen->setEnabled(true);
        actionStop->setEnabled(false);
        connect(actionListen, &QAction::triggered, this, [this, actionListen, actionStop]() {
            bool ok;
            int  roomid = QInputDialog::getInt(this, u"请输入房间号"_s, u"房间号"_s, 0, 1, std::numeric_limits<int>::max(), 1, &ok);
            if (ok) {
                actionListen->setEnabled(false);
                actionStop->setEnabled(true);
                qobject_cast<DanmakuTableModel *>(danmakuTableView_->model())->listen(roomid);
                setWindowTitle(windowTitle().prepend(QString::number(roomid) + " - "));
            }
        });
        connect(actionStop, &QAction::triggered, this, [this, actionListen, actionStop]() {
            qobject_cast<DanmakuTableModel *>(danmakuTableView_->model())->stop();
            actionListen->setEnabled(true);
            actionStop->setEnabled(false);
            setWindowTitle(u"弹幕客户端"_s);
        });
        // 当可执行文件名被修改为房间号时直接监听对应房间号
        QFileInfo info(QApplication::applicationFilePath());
        Q_ASSERT(info.isExecutable());
        bool ok;
        int  roomid = info.baseName().toInt(&ok);
        if (ok) {
            QTimer::singleShot(20, [this, actionListen, actionStop, roomid]() {
                actionListen->setEnabled(false);
                actionStop->setEnabled(true);
                qobject_cast<DanmakuTableModel *>(danmakuTableView_->model())->listen(roomid);
                setWindowTitle(windowTitle().prepend(QString::number(roomid) + " - "));
            });
        }
    }
    // 配置右键弹出菜单
    {
        danmakuTableView_->setContextMenuPolicy(Qt::CustomContextMenu);
        auto menu = new QMenu(this);
        connect(danmakuTableView_, &QTableView::customContextMenuRequested, menu, [this, menu](const QPoint &pos) { menu->popup(danmakuTableView_->mapToGlobal(pos)); });
        QAction *actionClear = menu->addAction(u"清屏"_s);
        actionClear->setEnabled(true);
        connect(actionClear, &QAction::triggered, qobject_cast<DanmakuTableModel *>(danmakuTableView_->model()), &DanmakuTableModel::clear);
        QAction *actionAlwaysScrollToBottom = menu->addAction(u"总是滚动到底部"_s);
        actionClear->setEnabled(true);
        actionAlwaysScrollToBottom->setCheckable(true);
        actionAlwaysScrollToBottom->setChecked(true);
        connect(danmakuTableView_->model(), &QAbstractItemModel::rowsInserted, danmakuTableView_, [this, actionAlwaysScrollToBottom]() {
            if (actionAlwaysScrollToBottom->isChecked()) danmakuTableView_->scrollToBottom();
        });
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget *w = centralWidget();
    danmakuTableView_->resize(w->width() - 4, w->height() - 4);
    return QMainWindow::resizeEvent(event);
}

#include "mainwindow.h"
#include "danmakuclient.h"
#include "danmakutablemodel.h"
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QFileInfo>
#include <QInputDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QResizeEvent>
#include <QScreen>
#include <QSettings>
#include <QSize>
#include <QStatusBar>
#include <QString>
#include <QTabWidget>
#include <QTableView>
#include <QTimer>
#include <QWidget>
#include <numeric>

using namespace Qt::Literals::StringLiterals;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      settings_(new QSettings(QSettings::IniFormat, QSettings::UserScope, u"hly1204"_s, u"danmaku"_s, this)),
      tabWidget_(new QTabWidget(this)),
      danmakuTableView_(new QTableView)
{
    setCentralWidget(new QWidget(this));
    centralWidget()->setObjectName(u"中央微件"_s);
    setMenuBar(new QMenuBar(this));
    menuBar()->setObjectName(u"菜单栏"_s);
    setStatusBar(new QStatusBar(this));
    statusBar()->setObjectName(u"状态栏"_s);
    setWindowTitle(u"弹幕机"_s);
    setMinimumSize(320, 240);

    DanmakuClient *danmakuClient = new DanmakuClient(this);
    tabWidget_->setParent(centralWidget());
    {
        DanmakuTableModel *danmakuTableModel = new DanmakuTableModel(this);
        danmakuTableModel->setDanmakuClient(danmakuClient);
        danmakuTableView_->setModel(danmakuTableModel);
    }
    tabWidget_->addTab(danmakuTableView_, u"弹幕"_s);

    // 读取配置
    {
        const QSize size = QApplication::primaryScreen()->virtualSize();
        int         w    = qBound(minimumWidth(), settings_->value("width"_L1, 640).toInt(), size.width());
        int         h    = qBound(minimumHeight(), settings_->value("height"_L1, 480).toInt(), size.height());
        int         x    = qBound(0, settings_->value("x"_L1, (size.width() - w) >> 1).toInt(), size.width() - w);
        int         y    = qBound(0, settings_->value("y"_L1, (size.height() - h) >> 1).toInt(), size.height() - h);
        setGeometry(x, y, w, h);
    }

    // 配置配置菜单
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
            setWindowTitle(u"弹幕机"_s);
        });

        bool ok;
        int  roomid = settings_->value("roomid", 0).toInt(&ok);
        if (ok && roomid != 0) {
            QTimer::singleShot(20, [this, actionListen, actionStop, roomid]() {
                actionListen->setEnabled(false);
                actionStop->setEnabled(true);
                qobject_cast<DanmakuTableModel *>(danmakuTableView_->model())->listen(roomid);
                setWindowTitle(windowTitle().prepend(QString::number(roomid) + " - "));
            });
        }
    }

    // 配置关于
    {
        QMenu   *menu        = menuBar()->addMenu(u"?"_s);
        QAction *actionAbout = menu->addAction(u"关于"_s);
        actionAbout->setEnabled(true);
        connect(actionAbout, &QAction::triggered, this, [this]() {
            QString text = u"%1\n作者: %2\n源代码: %3\n构建时间: %4"_s.arg(u"本软件遵循 GPL-3.0 协议"_s)
                               .arg(u"hly1204"_s)
                               .arg(u"github.com/hly1204/danmaku4bilibili")
                               .arg(QDateTime::currentDateTime().toString(Qt::ISODate));
            QMessageBox::about(this, u"关于弹幕机"_s, text);
        });
    }

    // 配置右键弹出菜单
    {
        danmakuTableView_->setContextMenuPolicy(Qt::CustomContextMenu);
        QMenu *menu = new QMenu(this);
        connect(danmakuTableView_, &QTableView::customContextMenuRequested, menu, [this, menu](const QPoint &pos) {
            menu->popup(danmakuTableView_->mapToGlobal(pos));
        });
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

    // 配置状态栏刷新人气值
    connect(danmakuClient, &DanmakuClient::popularityFlushed, this, [this](quint32 popularity) {
        statusBar()->showMessage(u"人气: %1"_s.arg(popularity));
    });
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event);
    QWidget *w = centralWidget();
    tabWidget_->resize(w->width() - 4, w->height() - 4);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    settings_->setValue("x", x());
    settings_->setValue("y", y());
    settings_->setValue("width", width());
    settings_->setValue("height", height());
    settings_->setValue("roomid", qobject_cast<DanmakuTableModel *>(danmakuTableView_->model())->roomid());
    settings_->sync();
    event->accept();
}
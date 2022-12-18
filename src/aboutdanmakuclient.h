#ifndef ABOUT_DANMAKU_CLIENT_H
#define ABOUT_DANMAKU_CLIENT_H

#include <QDialog>

// QT
class QPushButton;
class QLabel;

class AboutDanmakuClient : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDanmakuClient(QWidget *parent = nullptr);

private:
    Q_DISABLE_COPY_MOVE(AboutDanmakuClient)

private:
    QLabel      *authorLabel;
    QLabel      *sourceLabel;
    QLabel      *dateLabel;
    QLabel      *licenseLabel;
    QPushButton *okButton;
};

#endif

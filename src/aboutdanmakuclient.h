#ifndef ABOUT_DANMAKU_CLIENT_H
#define ABOUT_DANMAKU_CLIENT_H

#include <QDialog>

// QT
class QPushButton;
class QLabel;

class AboutDanmakuClient : public QDialog
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AboutDanmakuClient)

public:
    explicit AboutDanmakuClient(QWidget *parent = nullptr);

private:
    QLabel      *authorLabel;
    QLabel      *sourceLabel;
    QLabel      *dateLabel;
    QLabel      *licenseLabel;
    QPushButton *okButton;
};

#endif

#ifndef ABOUT_DANMAKU_CLIENT_H_
#define ABOUT_DANMAKU_CLIENT_H_

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
    QLabel      *authorLabel_;
    QLabel      *sourceLabel_;
    QLabel      *dateLabel_;
    QLabel      *licenseLabel_;
    QPushButton *okButton_;
};

#endif

#include "aboutdanmakuclient.h"
#include <QLabel>
#include <QPushButton>
#include <QString>

using namespace Qt::Literals::StringLiterals;

AboutDanmakuClient::AboutDanmakuClient(QWidget *parent)
    : QDialog(parent, Qt::Dialog),
      authorLabel_(new QLabel(this)),
      sourceLabel_(new QLabel(this)),
      dateLabel_(new QLabel(this)),
      licenseLabel_(new QLabel(this)),
      okButton_(new QPushButton(this))
{
    this->setWindowTitle(u"关于弹幕机"_s);
    this->setFixedSize(250, 140);
    this->setWindowFlags(this->windowFlags() & ~Qt::WindowCloseButtonHint);

    okButton_->setGeometry(90, 105, 70, 25);
    okButton_->setText(u"关闭"_s);
    authorLabel_->move(20, 15);
    authorLabel_->setText(u"作者: hly1204"_s);
    sourceLabel_->move(20, 35);
    sourceLabel_->setText(u"<a>源代码: </a><a style=text-decoration:none; href=\"https://github.com/hly1204/danmaku4bilibili/\">danmaku4bilibili (GitHub)</a>"_s);
    sourceLabel_->setOpenExternalLinks(true);
    dateLabel_->move(20, 55);
    dateLabel_->setText(u"构建时间: %1 %2"_s.arg(__DATE__).arg(__TIME__));
    licenseLabel_->move(20, 75);
    licenseLabel_->setText(u"本软件在 <a style=text-decoration:none; href=\"https://github.com/hly1204/danmaku4bilibili/blob/master/LICENSE/\">LICENSE</a> 下分发"_s);
    licenseLabel_->setOpenExternalLinks(true);

    connect(okButton_, &QPushButton::clicked, this, &AboutDanmakuClient::close);
}

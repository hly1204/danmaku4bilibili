#include "aboutdanmakuclient.h"
#include <QLabel>
#include <QPushButton>
#include <QString>

using namespace Qt::Literals::StringLiterals;

AboutDanmakuClient::AboutDanmakuClient(QWidget *parent)
    : QDialog(parent, Qt::Dialog),
      authorLabel(new QLabel(this)),
      sourceLabel(new QLabel(this)),
      dateLabel(new QLabel(this)),
      licenseLabel(new QLabel(this)),
      okButton(new QPushButton(this))
{
    setWindowTitle(u"关于弹幕机"_s);
    setFixedSize(250, 140);
    setWindowFlags(windowFlags() & ~Qt::WindowCloseButtonHint);

    okButton->setGeometry(90, 105, 70, 25);
    okButton->setText(u"关闭"_s);
    authorLabel->move(20, 15);
    authorLabel->setText(u"作者: hly1204"_s);
    sourceLabel->move(20, 35);
    sourceLabel->setText(u"<a>源代码: </a><a style=text-decoration:none; href=\"https://github.com/hly1204/danmaku4bilibili/\">danmaku4bilibili (GitHub)</a>"_s);
    dateLabel->move(20, 55);
    dateLabel->setText(u"构建时间: %1 %2"_s.arg(__DATE__).arg(__TIME__));
    licenseLabel->move(20, 75);
    licenseLabel->setText(u"本软件在 <a style=text-decoration:none; href=\"https://github.com/hly1204/danmaku4bilibili/blob/master/LICENSE/\">LICENSE</a> 下分发"_s);
    authorLabel->setOpenExternalLinks(true);

    connect(okButton, &QPushButton::clicked, this, &AboutDanmakuClient::close);
}

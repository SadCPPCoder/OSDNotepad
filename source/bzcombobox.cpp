/*******************************************
 * File Name: bzcombobox.cpp
 * Date: 2021-05-05
 * Author: Bob.Zhang
 *
 * Description: Implementation of a simple
 *   combobox.
 *******************************************/
#include "bzcombobox.h"
#include "ui_bzcombobox.h"
#include <QLabel>
#include <QMouseEvent>
#include <QPoint>
#include <QScrollBar>

BZCombobox::BZCombobox(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::BZCombobox),
    mListWgt(nullptr),
    mViewWgt(nullptr),
    mMouseClickOnBtn(false)
{
    ui->setupUi(this);

    ui->comboBtn->setGeometry(this->geometry());

    mListWgt = new QListWidget(this);
    mViewWgt = mListWgt;
    mListWgt->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    mListWgt->setTextElideMode(Qt::ElideMiddle);
    mListWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    mViewWgt->hide();
    mViewWgt->raise();
    mViewWgt->setWindowFlag(Qt::Popup, true);

    mViewWgt->installEventFilter(this);
}

BZCombobox::~BZCombobox()
{
    delete ui;
}

bool BZCombobox::eventFilter(QObject *watched, QEvent *event)
{
    if (QEvent::MouseButtonPress == event->type()&& watched == mViewWgt)
    {
        QMouseEvent* mouseEvent = static_cast<QMouseEvent*>(event);

        auto mPos = mouseEvent->globalPos();
        QPoint wPos = ui->comboBtn->mapToGlobal(QPoint(0,0));
        QRect wRect = ui->comboBtn->rect();
        if((mPos.x() > wPos.x() && mPos.x() < wPos.x() + wRect.width()) &&
           (mPos.y() > wPos.y() && mPos.y() < wPos.y() + wRect.height()))
        {
            mMouseClickOnBtn = true;
        }
        mViewWgt->hide();
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

QListWidget *BZCombobox::listWgt()
{
    return mListWgt;
}

void BZCombobox::setIcon(const QIcon &icon)
{
    ui->comboBtn->setIcon(icon);
}

void BZCombobox::on_comboBtn_clicked()
{
    if(mMouseClickOnBtn)
    {
        mMouseClickOnBtn = false;
        return;
    }

    auto gPos = mapToGlobal(QPoint(1,ui->comboBtn->height()));
    mViewWgt->move(gPos);

    emit updateListContent();

    // show/hide list widget
    mViewWgt->setVisible(!mViewWgt->isVisible());
}

/*******************************************
 * File Name: searchbar.h
 * Date: 2020-12-13
 * Author: Bob.Zhang
 *
 * Description: Implement the search bar widget
 * to used by the application.
 *******************************************/

#include "searchbar.h"
#include "ui_searchbar.h"
#include <QTextBlock>

SearchBar::SearchBar(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchBar),
    mEdit(NULL)
{
    ui->setupUi(this);
    ui->info->setStyleSheet("color:red");
}

SearchBar::~SearchBar()
{
    delete ui;
}

void SearchBar::setTextEdit(QTextEdit *edit)
{
    mEdit = edit;

    if(mEdit)
    {
        QPalette palette = mEdit->palette();
        palette.setColor(QPalette::Highlight,
                         palette.color(QPalette::Active,
                                       QPalette::Highlight));
        mEdit->setPalette(palette);
    }
}

void SearchBar::setVisible(bool visible)
{
    ui->lineEdit->setFocus();
    QWidget::setVisible(visible);
}

void SearchBar::setFocus()
{
    ui->lineEdit->setFocus();
}

bool SearchBar::tryFind(QString str)
{
    if(!mEdit)
        return false;

    QTextDocument::FindFlags flags;
    if(ui->chkCase->isChecked())
        flags = QTextDocument::FindCaseSensitively;

    bool ret = false;
    if(ui->chkRegx->isChecked())
    {
        auto cursor = mEdit->document()->find(QRegExp(str), 0, flags);
        ret = cursor.hasSelection();
    }
    else
    {

        auto cursor = mEdit->document()->find(str, 0, flags);
        ret = cursor.hasSelection();
    }

    return ret;
}

bool SearchBar::find(QString str)
{
    if(!mEdit)
        return false;

    QTextDocument::FindFlags flags;
    if(ui->chkCase->isChecked())
        flags = QTextDocument::FindCaseSensitively;

    bool ret = false;
    if(ui->chkRegx->isChecked())
    {
        ret = mEdit->find(QRegExp(str), flags);
    }
    else
    {
        ret = mEdit->find(str, flags);
    }

    return ret;
}

void SearchBar::on_lineEdit_returnPressed()
{
    on_findBtn_clicked();
}

void SearchBar::on_findBtn_clicked()
{
    static QString lastStr;
    QString str = ui->lineEdit->text();

    ui->info->setText("");

    if(lastStr != str)
    {
        // first find, should try at first
        if(!tryFind(str))
        {
            // notice not found the string
            ui->info->setText(tr("Can't find the string."));

            return;
        }

        lastStr = str;

        // move the text cursor to begin of the file.
        auto cursor = mEdit->textCursor();
        cursor.setPosition(0);
        mEdit->setTextCursor(cursor);
    }

    if(!find(str))
    {
        // notice the final string that matches.
        ui->info->setText(tr("End of the file!"));

        // move the text cursor to begin of the file.
        auto cursor = mEdit->textCursor();
        cursor.setPosition(0);
        mEdit->setTextCursor(cursor);
    }
}

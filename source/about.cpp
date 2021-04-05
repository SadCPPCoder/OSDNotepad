/*******************************************
 * File Name: about.cpp
 * Date: 2020-11-22
 * Author: Bob.Zhang
 *
 * Description: Implementation of about page
 *******************************************/

#include "about.h"
#include "globalinfo.h"
#include "ui_about.h"

About::About(QWidget *parent)
    : QDialog(parent,
              Qt::WindowTitleHint |
              Qt::CustomizeWindowHint |
              Qt::WindowCloseButtonHint)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
    auto title = windowTitle();
    title += QString::asprintf("_%d.%d", MAIN_VERSION_NO, SUB_VERSION_NO);
    setWindowTitle(title);
}

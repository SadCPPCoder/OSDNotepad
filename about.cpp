#include "about.h"
#include "ui_about.h"

About::About(QWidget *parent)
    : QDialog(parent,
              Qt::WindowTitleHint |
              Qt::CustomizeWindowHint |
              Qt::WindowCloseButtonHint)
    , ui(new Ui::Dialog)
{
    ui->setupUi(this);
}

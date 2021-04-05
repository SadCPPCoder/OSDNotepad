/*******************************************
 * File Name: about.h
 * Date: 2020-11-22
 * Author: Bob.Zhang
 *
 * Description: Declaration of about page
 *******************************************/

#ifndef ABOUT_H
#define ABOUT_H

#include <QDialog>

namespace Ui {
class Dialog;
}

class About: public QDialog
{
    Q_OBJECT
private:
    Ui::Dialog *ui;
public:
    About(QWidget *parent = 0);
};

#endif // ABOUT_H

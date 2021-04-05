/*******************************************
 * File Name: osdshortcutdialog.h
 * Date: 2021-03-07
 * Author: Bob.Zhang
 *
 * Description: Declaration of shortcut
 * keys dialog.
 *******************************************/

#ifndef OSDSHORTCUTDIALOG_H
#define OSDSHORTCUTDIALOG_H

#include <QDialog>

namespace Ui {
class OSDShortcutDialog;
}

class OSDShortcutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit OSDShortcutDialog(QWidget *parent = 0);
    ~OSDShortcutDialog();

private:
    Ui::OSDShortcutDialog *ui;
};

#endif // OSDSHORTCUTDIALOG_H

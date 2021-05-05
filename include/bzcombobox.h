/*******************************************
 * File Name: bzcombobox.h
 * Date: 2021-05-05
 * Author: Bob.Zhang
 *
 * Description: Declaration of a simple
 *   combobox.
 *******************************************/
#ifndef BZCOMBOBOX_H
#define BZCOMBOBOX_H

#include <QListWidget>
#include <QWidget>

namespace Ui {
class BZCombobox;
}

class BZCombobox : public QWidget
{
    Q_OBJECT

public:
    explicit BZCombobox(QWidget *parent = 0);
    ~BZCombobox();
    bool eventFilter(QObject *watched, QEvent *event);
    QListWidget *listWgt();
    void setIcon(const QIcon& icon);

private slots:
    void on_comboBtn_clicked();

signals:
    void updateListContent();

private:
    Ui::BZCombobox *ui;
    QListWidget *mListWgt;
    QAbstractItemView *mViewWgt;
    bool mMouseClickOnBtn;
};

#endif // BZCOMBOBOX_H

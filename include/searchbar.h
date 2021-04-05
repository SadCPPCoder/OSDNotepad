/*******************************************
 * File Name: searchbar.h
 * Date: 2020-12-13
 * Author: Bob.Zhang
 *
 * Description: Declare the search bar widget
 * to used by the application.
 *******************************************/

#ifndef SEARCHBAR_H
#define SEARCHBAR_H

#include <QTextEdit>
#include <QWidget>

namespace Ui {
class SearchBar;
}

class SearchBar : public QWidget
{
    Q_OBJECT

public:
    explicit SearchBar(QWidget *parent = 0);
    ~SearchBar();
    void setTextEdit(QTextEdit *edit);
    void setVisible(bool visible);
    void setFocus();

private:
    bool tryFind(QString str);
    bool find(QString str);

private slots:
    void on_findBtn_clicked();
    void on_lineEdit_returnPressed();

private:
    Ui::SearchBar *ui;
    QTextEdit *mEdit;
};

#endif // SEARCHBAR_H

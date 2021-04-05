/*******************************************
 * File Name: osdfontcombobox.h
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Declare the combobox widget
 * to set current font.
 *******************************************/

#ifndef OSDFONTCOMBOBOX_H
#define OSDFONTCOMBOBOX_H

#include <QFontComboBox>

class OSDFontComboBox : public QFontComboBox
{
    Q_OBJECT

public:
    OSDFontComboBox(QWidget *parent);
    void setCurrentFontFamily(const QString& family);

signals:
    void currentFontFamilyChanged(const QString& family);

private slots:
    void onCurrentTextChanged(const QString& text);

private:
    bool mTextChangedByUi;
};

#endif // OSDFONTCOMBOBOX_H

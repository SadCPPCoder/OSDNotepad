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

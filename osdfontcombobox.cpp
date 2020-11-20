#include "osdfontcombobox.h"

OSDFontComboBox::OSDFontComboBox(QWidget *parent)
    :QFontComboBox(parent)
    ,mTextChangedByUi(true)
{

    connect(this, SIGNAL(currentTextChanged(QString)), this, SLOT(onCurrentTextChanged(QString)));
}

void OSDFontComboBox::setCurrentFontFamily(const QString &family)
{
    // default signal slot schedule is immediatelly, so we can set the flag
    mTextChangedByUi = false;
    QFontComboBox::setCurrentText(family);
    mTextChangedByUi = true;
}

void OSDFontComboBox::onCurrentTextChanged(const QString &text)
{
    if(mTextChangedByUi)
    {
        emit currentFontFamilyChanged(text);
    }
}


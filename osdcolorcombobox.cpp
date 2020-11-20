#include "osdcolorcombobox.h"

OSDColorComboBox::OSDColorComboBox(QWidget *parent,
                                   OSDColorComboBoxColorIndexType colorIndex)
    :QComboBox(parent)
    ,mTextChangedByUi(true)
    ,mColorVec()
    ,mColorIconVec()
    ,mDefaultColor(colorIndex)
{
    if(mDefaultColor > OSDPerple)
    {
        mDefaultColor = OSDWhite;
    }

    mColorVec.insert(OSDBlack, "#000000");
    mColorVec.insert(OSDWhite, "#FFFFFF");
    mColorVec.insert(OSDGray, "#808080");
    mColorVec.insert(OSDRed, "#FF0000");
    mColorVec.insert(OSDPink, "#FFC0CB");
    mColorVec.insert(OSDBrown, "#A52A2A");
    mColorVec.insert(OSDOrange, "#FFA500");
    mColorVec.insert(OSDYellow, "#FFFF00");
    mColorVec.insert(OSDGreen, "#008000");
    mColorVec.insert(OSDBlue, "#0000FF");
    mColorVec.insert(OSDPerple, "#800080");

    mColorIconVec.insert(OSDBlack, QIcon(":/res/color/black.png"));
    mColorIconVec.insert(OSDWhite, QIcon(":/res/color/white.png"));
    mColorIconVec.insert(OSDGray, QIcon(":/res/color/gray.png"));
    mColorIconVec.insert(OSDRed, QIcon(":/res/color/red.png"));
    mColorIconVec.insert(OSDPink, QIcon(":/res/color/pink.png"));
    mColorIconVec.insert(OSDBrown, QIcon(":/res/color/brown.png"));
    mColorIconVec.insert(OSDOrange, QIcon(":/res/color/orange.png"));
    mColorIconVec.insert(OSDYellow, QIcon(":/res/color/yellow.png"));
    mColorIconVec.insert(OSDGreen, QIcon(":/res/color/green.png"));
    mColorIconVec.insert(OSDBlue, QIcon(":/res/color/blue.png"));
    mColorIconVec.insert(OSDPerple, QIcon(":/res/color/perple.png"));

    for(int i=0; i<= OSDPerple; ++i)
    {
        insertItem(i, mColorIconVec[i], "", mColorVec[i]);
    }
    setCurrentIndex(mDefaultColor);

    connect(this, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

void OSDColorComboBox::setCurrentColor(const QColor& color)
{
    // default signal slot schedule is immediatelly, so we can set the flag
    mTextChangedByUi = false;

    auto colorName = color.name().toUpper();
    int colorIndex = mDefaultColor;
    if(mColorVec.contains(colorName))
    {
        colorIndex = mColorVec.indexOf(colorName);
    }

    QComboBox::setCurrentIndex(colorIndex);
    mTextChangedByUi = true;
}

void OSDColorComboBox::onCurrentIndexChanged(int index)
{
    if(mTextChangedByUi)
    {
        QColor color(itemData(index).value<QString>());
        emit currentColorChanged(color);
    }

}

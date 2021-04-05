/*******************************************
 * File Name: osdspinbox.h
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Implement the spinbox widget
 * to used by main window.
 *******************************************/

#include "globalinfo.h"
#include "osdspinbox.h"

OSDSpinBox::OSDSpinBox(QWidget* parent)
    :QSpinBox(parent)
    ,mValueChangedByUi(true)
{
    setMinimum(FONT_MIN_POINT_SIZE);
    setMaximum(FONT_MAX_POINT_SIZE);
    setSingleStep(2);
    setValue(DEFAULT_FONT_POINT_SIZE);
    setPrefix("Size:");

    connect(this, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));
}

void OSDSpinBox::setCurrentValue(int value)
{
    // default signal slot schedule is immediatelly, so we can set the flag
    mValueChangedByUi = false;
    QSpinBox::setValue(value);
    mValueChangedByUi = true;
}

void OSDSpinBox::onValueChanged(int value)
{
    if(mValueChangedByUi)
    {
        emit currentValueChanged(value);
    }
}

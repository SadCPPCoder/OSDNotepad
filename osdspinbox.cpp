#include "osdspinbox.h"

OSDSpinBox::OSDSpinBox(QWidget* parent)
    :QSpinBox(parent)
    ,mValueChangedByUi(true)
{
    setMinimum(8);
    setMaximum(38);
    setSingleStep(2);
    setValue(14);
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

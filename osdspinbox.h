#ifndef OSDSPINBOX_H
#define OSDSPINBOX_H

#include <QSpinBox>

class OSDSpinBox : public QSpinBox
{
    Q_OBJECT

public:
    OSDSpinBox(QWidget* parent);
    void setCurrentValue(int value);

signals:
    void currentValueChanged(int value);

private slots:
    void onValueChanged(int value);

private:
    bool mValueChangedByUi;
};

#endif // OSDSPINBOX_H

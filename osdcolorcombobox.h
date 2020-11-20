#ifndef OSDCOMBOBOX_H
#define OSDCOMBOBOX_H

#include <QComboBox>
#include <QVector>

class OSDColorComboBox : public QComboBox
{
    Q_OBJECT

public:
    enum OSDColorComboBoxColorIndexType
    {
        OSDBlack=0,
        OSDWhite,
        OSDGray,
        OSDRed,
        OSDPink,
        OSDBrown,
        OSDOrange,
        OSDYellow,
        OSDGreen,
        OSDBlue,
        OSDPerple,
    };

public:
    OSDColorComboBox(QWidget *parent,
                     OSDColorComboBoxColorIndexType colorIndex);
    void setCurrentColor(const QColor& color);

signals:
    void currentColorChanged(const QColor& color);

private slots:
    void onCurrentIndexChanged(int index);

private:
    bool mTextChangedByUi;
    QVector<QString> mColorVec;
    QVector<QIcon> mColorIconVec;
    OSDColorComboBoxColorIndexType mDefaultColor;
};

#endif // OSDCOMBOBOX_H

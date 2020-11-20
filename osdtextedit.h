/******************************************
 * OSDTextEdit Defination
 *
 * Description: This file is to define the
 * OSD text edit for OSD file.
 *
 * Author: SadCPPCoder
 * Date: 2020-10-23
 * License: GPL
 ******************************************/

#ifndef OSDTEXTEDIT_H
#define OSDTEXTEDIT_H

#include <QColor>
#include <QImage>
#include <QTextEdit>

class OSDTextEdit : public QTextEdit
{
    Q_OBJECT

private:
    enum OSDTextEditFlagType
    {
        SmallImageModeFlag=0,
    };

    struct ImageSize
    {
        int width;
        int height;
    };

public:
    OSDTextEdit(QWidget *parent);

    void insertImage(const QImage& img);
    // TODO: table and list.

    void setOSD(const QString& osd);
    QString toOSD();
    bool isFontBold();
    bool isFontItalic();
    bool isFontUnderline();
    bool isFontDeleteline();
    QColor fontForegroundColor();
    QColor fontBackgroundColor();
    QString fontFamily();
    int fontSize();
    bool isSmallImageMode();
    void resetFontFormat();

public slots:
    void setFontBold(bool bold);
    void setFontItalic(bool italic);
    void setFontUnderline(bool underline);
    void setFontDeleteline(bool deleteline);
    void setFontForegroundColor(const QColor& color);
    void setFontBackgroundColor(const QColor& color);
    void setFontFamily(const QString& family);
    void setFontSize(int size);
    void setSmallImageMode(int maxHeight);

private:
    bool testFlag(OSDTextEditFlagType type);
    void setFlag(OSDTextEditFlagType type, bool value);
    void travelToResizeImage(QTextFrame *frame, bool isSmall);
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);

private:
    quint32 mFlags;
    int mMaxImageHeight;

    QMap<QString, ImageSize> mImageOriginSizeMap;
};

#endif // OSDTEXTEDIT_H

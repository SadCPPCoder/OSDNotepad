/******************************************
 * OSDTextEdit Implementation
 *
 * Description: This file is to implement
 * the OSD text edit for OSD file.
 *
 * Author: SadCPPCoder
 * Date: 2020-10-23
 * License: GPL
 ******************************************/

#include "osdtextedit.h"

#include <QBuffer>
#include <QImageReader>
#include <QTextFrame>
#include <QTextBlock>
#include <QMimeData>
#include <QDebug>
#include <QCryptographicHash>

#define DEFALUT_MAX_IMG_HEIGHT (100)
#define FONT_MIN_POINT_SIZE (8)
#define FONT_MAX_POINT_SIZE (34)
#define DEFAULT_FONT_POINT_SIZE (14)

static const QString fileName("test.osd");
static const QString infoDataStart("\n<!--Info\n");
static const QString infoDataStop("Info-->\n");
static const QString smallImageInfo = "smallImg";
static const QString infoInternalSep = "$";
static const QString infoSep = "\n";
static const QString imageDataStart("\n<!--Image\n");
static const QString imageDataStop("Image-->\n");
static const QString imgInternalSep = "%";
static const QString imgSep = "\n";

OSDTextEdit::OSDTextEdit(QWidget *parent)
    : QTextEdit(parent)
    , mFlags(0)
    , mMaxImageHeight(DEFALUT_MAX_IMG_HEIGHT)
    , mImageOriginSizeMap()
{
    setTextColor(Qt::black);
    setTextBackgroundColor(Qt::white);
}

void OSDTextEdit::insertImage(const QImage &img)
{
    int height = img.height();
    int width = img.width();
    if(testFlag(SmallImageModeFlag) && height > mMaxImageHeight)
    {
        float ratio = (float)mMaxImageHeight / height;
        height = mMaxImageHeight;
        width = width * ratio;
    }

    // calculate hash code
    QByteArray ba;
    QBuffer buffer(&ba);
    buffer.open(QIODevice::WriteOnly);
    img.save(&buffer, "PNG");
    QByteArray hash = QCryptographicHash::hash(ba, QCryptographicHash::Md5);
    QString name = hash.toHex();

    QTextCursor cursor = textCursor();
    auto charFormat = cursor.charFormat();

    QTextImageFormat imgFormat;
    imgFormat.setWidth(width);
    imgFormat.setHeight(height);
    imgFormat.merge(charFormat);
    imgFormat.setName(name);

    document()->addResource(
                QTextDocument::ImageResource, QUrl(name), img);

    cursor.insertImage(imgFormat);

    mImageOriginSizeMap[name] = ImageSize{img.width(), img.height()};
}

void OSDTextEdit::setOSD(const QString &osd)
{
    auto indexImg = osd.lastIndexOf(imageDataStart);
    auto indexInfo = osd.lastIndexOf(infoDataStart);
    if(-1 != indexImg || -1 != indexInfo)
    {
        auto html = osd.midRef(0, indexInfo);
        auto info = osd.midRef(indexInfo + infoDataStart.size(),
                               indexImg - indexInfo - infoDataStart.size());
        auto image = osd.midRef(indexImg + imageDataStart.size());

        auto infoList = info.split(infoSep);

        auto infoItr = infoList.begin();
        for(; infoItr != infoList.end(); ++infoItr)
        {
            auto sepIndex = infoItr->indexOf(infoInternalSep);
            if(-1 != sepIndex)
            {
                auto name = infoItr->mid(0, sepIndex).toString();
                auto data = infoItr->mid(sepIndex + 1).toString();

                if(smallImageInfo == name)
                {
                    setFlag(SmallImageModeFlag, data == "1");
                }
            }
        }

        auto imageList = image.split(imgSep);
        auto itr = imageList.begin();
        for(; itr != imageList.end(); ++itr)
        {
            auto sepIndex = itr->indexOf(imgInternalSep);
            if(-1 != sepIndex)
            {
                auto name = itr->mid(0, sepIndex).toString();
                auto data = itr->mid(sepIndex + 1).toString();
                QByteArray ba = QByteArray::fromBase64(data.toUtf8());
                QBuffer buffer(&ba);
                QImage img = QImageReader(&buffer).read();
                document()->addResource(
                            QTextDocument::ImageResource, QUrl(name), img);

                mImageOriginSizeMap[name] =
                        ImageSize{img.width(), img.height()};
            }
        }

        setHtml(html.toString());

        travelToResizeImage(document()->rootFrame(), testFlag(SmallImageModeFlag));
    }
    else
    {
        setHtml(osd);
    }
}

QString OSDTextEdit::toOSD()
{
    QString osd;

    osd.append(toHtml());

    osd.append(infoDataStart);
    osd.append(smallImageInfo);
    osd.append(infoInternalSep);
    osd.append(testFlag(SmallImageModeFlag) ? "1" : "0");
    osd.append(infoSep);
    osd.append(infoDataStop);

    osd.append(imageDataStart);
    QStringList imgSaved;
    auto formats = document()->allFormats();
    for(auto itr = formats.begin(); itr!=formats.end(); ++itr)
    {
        if(itr->isImageFormat())
        {
            QString name = itr->toImageFormat().name();
            if(!imgSaved.contains(name))
            {
                QImage img = document()->resource(
                            QTextDocument::ImageResource,
                            QUrl(name)).value<QImage>();
                QByteArray ba;
                QBuffer buffer(&ba);
                buffer.open(QIODevice::WriteOnly);
                img.save(&buffer, "PNG");

                osd.append(name);
                osd.append(imgInternalSep);
                osd.append(ba.toBase64());
                osd.append(imgSep);
                imgSaved.append(name);
            }
        }
    }
    osd.append(imageDataStop);

    return osd;
}

bool OSDTextEdit::isFontBold()
{
    return currentFont().bold();
}

bool OSDTextEdit::isFontItalic()
{
    return currentFont().italic();
}

bool OSDTextEdit::isFontUnderline()
{
    return currentFont().underline();
}

bool OSDTextEdit::isFontDeleteline()
{
    return currentFont().strikeOut();
}

QColor OSDTextEdit::fontForegroundColor()
{
    return textColor();
}

QColor OSDTextEdit::fontBackgroundColor()
{
    return textBackgroundColor();
}

QString OSDTextEdit::fontFamily()
{
    return currentFont().family();
}

int OSDTextEdit::fontSize()
{
    return currentFont().pointSize();
}

bool OSDTextEdit::isSmallImageMode()
{
    return testFlag(SmallImageModeFlag);
}

void OSDTextEdit::resetFontFormat()
{
    auto font = currentFont();
    font.setBold(false);
    font.setItalic(false);
    font.setUnderline(false);
    font.setStrikeOut(false);
    font.setPointSize(DEFAULT_FONT_POINT_SIZE);
    font.setFamily(font.defaultFamily());
    setCurrentFont(font);
    setTextColor(Qt::black);
    setTextBackgroundColor(Qt::white);
}

void OSDTextEdit::setFontBold(bool bold)
{
    auto font = currentFont();
    font.setBold(bold);
    setCurrentFont(font);
}

void OSDTextEdit::setFontItalic(bool italic)
{
    auto font = currentFont();
    font.setItalic(italic);
    setCurrentFont(font);
}

void OSDTextEdit::setFontUnderline(bool underline)
{
    auto font = currentFont();
    font.setUnderline(underline);
    setCurrentFont(font);
}

void OSDTextEdit::setFontDeleteline(bool deleteline)
{
    auto font = currentFont();
    font.setStrikeOut(deleteline);
    setCurrentFont(font);
}

void OSDTextEdit::setFontForegroundColor(const QColor &color)
{
    setTextColor(color);
}

void OSDTextEdit::setFontBackgroundColor(const QColor &color)
{
    setTextBackgroundColor(color);
}

void OSDTextEdit::setFontFamily(const QString &family)
{
    auto font = currentFont();
    font.setFamily(family);
    setCurrentFont(font);
}

void OSDTextEdit::setFontSize(int size)
{
    auto font = currentFont();
    font.setPointSize(size);
    setCurrentFont(font);
}

void OSDTextEdit::setSmallImageMode(int maxHeight)
{
    setFlag(SmallImageModeFlag, maxHeight>0);

    travelToResizeImage(document()->rootFrame(), testFlag(SmallImageModeFlag));
}

bool OSDTextEdit::testFlag(OSDTextEdit::OSDTextEditFlagType type)
{
    return mFlags & (1 >> type);
}

void OSDTextEdit::setFlag(OSDTextEdit::OSDTextEditFlagType type, bool value)
{
    if(value)
        mFlags |= (1>>type);
    else
        mFlags &= (~(1>>type));
}

void OSDTextEdit::travelToResizeImage(QTextFrame *frame, bool isSmall)
{
    if(!frame)
    {
        return;
    }

    for(auto itrFrame=frame->begin(); !itrFrame.atEnd(); ++itrFrame)
    {
        if(itrFrame.currentBlock().isValid())
        {
            for(auto itrBlock = itrFrame.currentBlock().begin();
                !itrBlock.atEnd(); ++itrBlock)
            {
                auto blockFragment = itrBlock.fragment();
                auto imgFormat = blockFragment.charFormat().toImageFormat();
                if(imgFormat.isValid())
                {
                    auto height = imgFormat.height();

                    if(isSmall && height <= mMaxImageHeight)
                    {
                        continue;
                    }

                    auto width = imgFormat.width();

                    if(isSmall && height > 100)
                    {
                        float ratio = (float)mMaxImageHeight / height;
                        height = mMaxImageHeight;
                        width = width * ratio;
                    }
                    else if(!isSmall)
                    {
                        auto name = imgFormat.name();
                        width = mImageOriginSizeMap[name].width;
                        height = mImageOriginSizeMap[name].height;
                    }

                    imgFormat.setWidth(width);
                    imgFormat.setHeight(height);

                    QTextCursor cursor(itrFrame.currentBlock());
                    cursor.setPosition(blockFragment.position());
                    cursor.setPosition(blockFragment.position() +
                                       blockFragment.length(),
                                        QTextCursor::KeepAnchor);
                    cursor.setCharFormat(imgFormat);
                }
            }
        }
        else if(itrFrame.currentFrame())
        {
            travelToResizeImage(itrFrame.currentFrame(), isSmall);
        }
    }
}

bool OSDTextEdit::canInsertFromMimeData(const QMimeData *source) const
{
    if (source->hasImage())
        return true;
    else
        return QTextEdit::canInsertFromMimeData(source);
}

void OSDTextEdit::insertFromMimeData(const QMimeData *source)
{
    if (source->hasImage())
    {
        QImage img = qvariant_cast<QImage>(source->imageData());

        insertImage(img);
    }
    else
        QTextEdit::insertFromMimeData(source);
}


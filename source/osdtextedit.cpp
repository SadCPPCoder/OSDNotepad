/*******************************************
 * File Name: osdtextedit.h
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Implement the textedit widget
 * to used by the application.
 *******************************************/

#include "globalinfo.h"
#include "osdconfig.h"
#include "osdtextedit.h"
#include <QAbstractTextDocumentLayout>
#include <QApplication>
#include <QBuffer>
#include <QClipboard>
#include <QCryptographicHash>
#include <QFileDialog>
#include <QFileInfo>
#include <QImageReader>
#include <QMenu>
#include <QMimeData>
#include <QPainter>
#include <QPrinter>
#include <QScrollBar>
#include <QTextBlock>
#include <QTextDocumentFragment>
#include <QTextFrame>

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
    , mMaxImageHeight(DEFAULT_MAX_IMG_HEIGHT)
    , mMaxImageWidth(DEFAULT_MAX_IMG_WIDTH)
    , mImageOriginSizeMap()
{
    setTextColor(Qt::black);
    setTextBackgroundColor(Qt::white);
    setTabStopWidth(40);
    auto defaultFont = font();
    defaultFont.setPointSize(DEFAULT_FONT_POINT_SIZE);
    document()->setDefaultFont(defaultFont);
    setContextMenuPolicy(Qt::CustomContextMenu);

    connect(this, SIGNAL(cursorPositionChanged()),
            this, SLOT(on_cursorPositionChanged()));
    connect(this, SIGNAL(selectionChanged()),
            this, SLOT(on_selectionChanged()));
    connect(this, SIGNAL(textChanged()),
            this, SLOT(on_textChanged()));
    connect(this, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(on_customContextMenuRequested(QPoint)));

    // initial value as modified
    setFlag(FileModifiedFlag, true);
    setAcceptDrops(false);
}

void OSDTextEdit::insertImage(const QImage &img)
{
    int height = img.height();
    int width = img.width();
    if(testFlag(SmallImageModeFlag))
    {
        if(height > mMaxImageHeight)
        {
            float ratio = (float)mMaxImageHeight / height;
            height = mMaxImageHeight;
            width *= ratio;
        }

        if(width > mMaxImageWidth)
        {
            float ratio = (float)mMaxImageWidth / width;
            height *= ratio;
            width = mMaxImageWidth;
        }
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
    imgFormat.merge(charFormat);
    imgFormat.setWidth(width);
    imgFormat.setHeight(height);
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

        travelToResizeImage(document()->rootFrame(), testFlag(SmallImageModeFlag),
                            mMaxImageHeight, mMaxImageWidth);
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

bool OSDTextEdit::isFontBold() const
{
    return currentFont().bold();
}

bool OSDTextEdit::isFontItalic() const
{
    return currentFont().italic();
}

bool OSDTextEdit::isFontUnderline() const
{
    return currentFont().underline();
}

bool OSDTextEdit::isFontDeleteline() const
{
    return currentFont().strikeOut();
}

QColor OSDTextEdit::fontForegroundColor() const
{
    return textColor();
}

QColor OSDTextEdit::fontBackgroundColor() const
{
    return textBackgroundColor();
}

QString OSDTextEdit::fontFamily() const
{
    return currentFont().family();
}

int OSDTextEdit::fontSize() const
{
    return currentFont().pointSize();
}

bool OSDTextEdit::isSmallImageMode() const
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
    setCurrentFont(font);
    setTextColor(Qt::black);
    setTextBackgroundColor(Qt::white);

    // Reset line height
    QTextBlockFormat format = textCursor().blockFormat();
    format.setLineHeight(DEFAULT_LINE_HEIGHT,
                         DEFAULT_LINE_HEIGHT_TYPE);
    QTextCursor curCursor = textCursor();
    curCursor.mergeBlockFormat(format);
}

void OSDTextEdit::setFontAttributes(OSDTextEdit::FontAttributeType type, QVariant value)
{
    if(FONT_ATTR_START >= type || FONT_ATTR_END <= type)
    {
        return;
    }

    QTextCursor cursor = textCursor();
    bool selectFlag = cursor.selectionStart() != cursor.selectionEnd();

    if(selectFlag)
    {
        QTextCursor fmtCursor(document());
        fmtCursor.beginEditBlock();
        for(int i=cursor.selectionStart(); i<cursor.selectionEnd(); ++i)
        {
            fmtCursor.setPosition(i);
            fmtCursor.setPosition(i+1, QTextCursor::KeepAnchor);
            auto fmt = fmtCursor.charFormat();
            auto font = fmt.font();
            if(FONT_BOLD == type)
                font.setBold(value.toBool());
            else if(FONT_ITALIC == type)
                font.setItalic(value.toBool());
            else if(FONT_DELETELINE == type)
                font.setStrikeOut(value.toBool());
            else if(FONT_UNDERLINE == type)
                font.setUnderline(value.toBool());
            else if(FONT_SIZE == type)
                font.setPointSize(value.toInt());
            else if(FONT_FAMILY == type)
                font.setFamily(value.toString());
            fmt.setFont(font);
            fmtCursor.setCharFormat(fmt);
        }
        fmtCursor.endEditBlock();
    }
    else
    {
        auto font = currentFont();
        if(FONT_BOLD == type)
            font.setBold(value.toBool());
        else if(FONT_ITALIC == type)
            font.setItalic(value.toBool());
        else if(FONT_DELETELINE == type)
            font.setStrikeOut(value.toBool());
        else if(FONT_UNDERLINE == type)
            font.setUnderline(value.toBool());
        else if(FONT_SIZE == type)
            font.setPointSize(value.toInt());
        else if(FONT_FAMILY == type)
            font.setFamily(value.toString());
        setCurrentFont(font);
    }
}

bool OSDTextEdit::openFromFile(const QString &fileName)
{
    if(fileName.isEmpty())
    {
        return false;
    }

    mCurFile.setFileName(fileName);
    mCurFile.open(QIODevice::ReadOnly);

    if(fileName.contains(".osd", Qt::CaseInsensitive))
    {
        setOSD(mCurFile.readAll());
    }
    else
    {
        setPlainText(mCurFile.readAll());
    }

    mCurFile.close();
    setFlag(FileModifiedFlag, false);

    return true;
}

bool OSDTextEdit::saveToFile(const QString &fileName)
{
    if(fileName.isEmpty())
    {
        return false;
    }

    QFileInfo fileInfo(fileName);    
    auto fileSuffix = fileInfo.suffix();
    
    mCurFile.setFileName(fileName + (fileSuffix.isEmpty() ? ".osd" : ""));

    return saveToFile();
}

bool OSDTextEdit::saveToFile()
{
    QFileInfo fileInfo(mCurFile);
    mCurFile.open(QIODevice::WriteOnly);

    if(!fileInfo.suffix().compare("osd", Qt::CaseInsensitive))
    {
        mCurFile.write(toOSD().toUtf8());
    }
    else
    {
        mCurFile.write(toPlainText().toUtf8());
    }

    mCurFile.close();

    setFlag(FileModifiedFlag, false);

    return true;
}

bool OSDTextEdit::isFileModified() const
{
    return testFlag(FileModifiedFlag);
}

bool OSDTextEdit::isFileExist() const
{
    return mCurFile.exists();
}

QString OSDTextEdit::fileName() const
{
    return mCurFile.fileName();
}

bool OSDTextEdit::printPDF(const QString fileName)
{
    OSDTextEdit edit(this);
    edit.setOSD(toOSD());
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(fileName);

    travelToResizeImage(edit.document()->rootFrame(), true,
                        printer.height()*0.8, printer.width()*0.8);

    edit.print(&printer);

    return true;
}

void OSDTextEdit::paintEvent(QPaintEvent *event)
{
    QTextEdit::paintEvent(event);

    QPainter painter(viewport());
    QColor color(Qt::gray);
    color.setAlpha(50);
    QPen pen;
    pen.setWidth(1);
    pen.setColor(color);
    painter.setPen(pen);
    QBrush brush;
    brush.setColor(color);
    brush.setStyle(Qt::SolidPattern);
    painter.setBrush(brush);
    QRect line(0, cursorRect().y(), width(), cursorRect().height());
    painter.drawRect(line);
}

void OSDTextEdit::keyPressEvent(QKeyEvent *event)
{
    QKeySequence keySeq(event->modifiers() + event->key());
    QString indentStr;
    // Shield the shortcut of shift+enter.
    if(event->key() == Qt::Key_Return &&
       event->modifiers() == Qt::ShiftModifier)
    {
        return;
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                      .getKeySequence(SK_TextRShift)) &&
            textCursor().hasSelection())
    {
        tabOperation(true);
        return;
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                      .getKeySequence(SK_TextLShift)))
    {
        tabOperation(false);
        return;
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                      .getKeySequence(SK_SmartIndent)))
    {
        indentStr = textCursor().block().text();
        int indentPos = indentStr.indexOf(QRegExp("(\\S)"));
        if(0 < indentPos)
        {
            indentStr.resize(indentPos);
        }
        else
        {
            indentStr.clear();
        }
    }

    QTextEdit::keyPressEvent(event);

    if(!indentStr.isEmpty())
    {
        textCursor().insertText(indentStr);
    }
}

void OSDTextEdit::resizeEvent(QResizeEvent *e)
{
    QTextEdit::resizeEvent(e);

    updateBottomBlankArea();
}

void OSDTextEdit::tabOperation(bool isAdd)
{
    QTextCursor cursor(document());
    cursor.setPosition(textCursor().selectionStart());
    auto block = cursor.block();
    int start = cursor.blockNumber();
    cursor.setPosition(textCursor().selectionEnd());
    int end = cursor.blockNumber();

    for(int i = start; i <= end; ++i)
    {
        QTextCursor curs(block);
        curs.movePosition(QTextCursor::StartOfBlock);
        if(isAdd)
        {
            curs.insertText("\t");
        }
        else
        {
            curs.movePosition(QTextCursor::Right, QTextCursor::KeepAnchor, 1);
            QString text = curs.selectedText();
            if(" " == text || "\t" == text)
            {
                curs.deleteChar();
            }
        }
        block = block.next();
    }
}

void OSDTextEdit::updateBottomBlankArea()
{
    auto frameFmt = document()->rootFrame()->frameFormat();
    qreal blankHeight = contentsRect().height() - 20;
    if(frameFmt.bottomMargin() != blankHeight)
    {
        frameFmt.setBottomMargin(blankHeight);
        setFlag(UpdateBottomBlankAreaFlag, true);
        document()->rootFrame()->setFrameFormat(frameFmt);
    }
}

void OSDTextEdit::setFontBold(bool bold)
{
    setFontAttributes(FONT_BOLD, bold);
}

void OSDTextEdit::setFontItalic(bool italic)
{
    setFontAttributes(FONT_ITALIC, italic);
}

void OSDTextEdit::setFontUnderline(bool underline)
{
    setFontAttributes(FONT_UNDERLINE, underline);
}

void OSDTextEdit::setFontDeleteline(bool deleteline)
{
    setFontAttributes(FONT_DELETELINE, deleteline);
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
    setFontAttributes(FONT_FAMILY, family);
}

void OSDTextEdit::setFontSize(int size)
{
    setFontAttributes(FONT_SIZE, size);
}

void OSDTextEdit::setSmallImageMode(int maxHeight)
{
    setFlag(SmallImageModeFlag, maxHeight>0);

    travelToResizeImage(document()->rootFrame(), testFlag(SmallImageModeFlag),
                        mMaxImageHeight, mMaxImageWidth);

    ensureCursorVisible();
}

void OSDTextEdit::on_cursorPositionChanged()
{
    viewport()->update();
}

void OSDTextEdit::on_selectionChanged()
{
    static QString lastStrFind;
    QList<QTextEdit::ExtraSelection> extralSelList;
    QString strFind = textCursor().selectedText();

    if(lastStrFind == strFind)
    {
        return;
    }

    if(!isReadOnly() &&
        textCursor().hasSelection())
    {
        auto result = document()->find(strFind, 0);
        while(result.hasSelection())
        {
            QTextEdit::ExtraSelection selection;

            QColor selColor = QColor(Qt::yellow).lighter(160);

            selection.format.setBackground(selColor);
            selection.cursor = result;
            extralSelList.append(selection);

            result = document()->find(strFind, result);
        }
    }

    lastStrFind = strFind;
    setExtraSelections(extralSelList);
}

void OSDTextEdit::on_textChanged()
{
    if(testFlag(UpdateBottomBlankAreaFlag))
    {
        setFlag(UpdateBottomBlankAreaFlag, false);
        return;
    }

    setFlag(FileModifiedFlag, true);
}

void OSDTextEdit::on_customContextMenuRequested(const QPoint& point)
{
    QPoint realPoint(point);
    realPoint.setX(realPoint.x() + horizontalScrollBar()->value());
    realPoint.setY(realPoint.y() + verticalScrollBar()->value());

    auto imgName = document()->documentLayout()->imageAt(realPoint);

    QMenu *menu = createStandardContextMenu();
    if(!imgName.isEmpty())
    {
        mClickedImgName = imgName;
        QAction *saveImgAction = new QAction(tr("Save Image..."), menu);
        connect(saveImgAction, SIGNAL(triggered()), this, SLOT(on_saveImage()));
        menu->addAction(saveImgAction);
        QAction *copyImgAction = new QAction(tr("Copy Image"), menu);
        connect(copyImgAction, SIGNAL(triggered()), this, SLOT(on_copyImage()));
        menu->addAction(copyImgAction);
    }
    menu->exec(QCursor::pos());
    delete menu;
}

void OSDTextEdit::on_saveImage()
{
    QFileInfo fileInfo(mCurFile);
    QString curDir = fileInfo.absolutePath();
    if(!fileInfo.exists())
    {
        curDir = QCoreApplication::applicationDirPath();
    }

    QString fileName = QFileDialog::getSaveFileName(this,
                                                tr("Save Image"),
                                                curDir,
                                                tr("Image (*.png)"));
    if(!fileName.isEmpty())
    {
        QImage img = document()->resource(
                    QTextDocument::ImageResource,
                    QUrl(mClickedImgName)).value<QImage>();
        img.save(fileName, "PNG", 100);
    }
}

void OSDTextEdit::on_copyImage()
{
    QImage img = document()->resource(
                QTextDocument::ImageResource,
                QUrl(mClickedImgName)).value<QImage>();

    QApplication::clipboard()->setImage(img);
}

bool OSDTextEdit::testFlag(OSDTextEdit::OSDTextEditFlagType type) const
{
    return mFlags & (1 << type);
}

void OSDTextEdit::setFlag(OSDTextEdit::OSDTextEditFlagType type, bool value)
{
    if(value)
        mFlags |= (1<<type);
    else
        mFlags &= (~(1<<type));
}

void OSDTextEdit::travelToResizeImage(QTextFrame *frame, bool isSmall,
                                      int height, int width)
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
                    auto imgHeight = imgFormat.height();
                    auto imgWidth = imgFormat.width();

                    if(isSmall && imgHeight <= height &&
                       imgWidth <= width)
                    {
                        continue;
                    }

                    if(isSmall)
                    {
                        if(imgHeight > height)
                        {
                            float ratio = (float)height / imgHeight;
                            imgHeight = height;
                            imgWidth *= ratio;
                        }

                        if(imgWidth > width)
                        {
                            float ratio = (float)width / imgWidth;
                            imgHeight *= ratio;
                            imgWidth = width;
                        }
                    }
                    else
                    {
                        auto name = imgFormat.name();
                        if(imgWidth == mImageOriginSizeMap[name].width &&
                           imgHeight == mImageOriginSizeMap[name].height)
                        {
                            continue;
                        }

                        imgWidth = mImageOriginSizeMap[name].width;
                        imgHeight = mImageOriginSizeMap[name].height;
                    }

                    imgFormat.setWidth(imgWidth);
                    imgFormat.setHeight(imgHeight);

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
            travelToResizeImage(itrFrame.currentFrame(), isSmall, height, width);
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


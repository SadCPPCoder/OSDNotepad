/*******************************************
 * File Name: osdtextedit.h
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Declare the textedit widget
 * to used by the application.
 *******************************************/

#ifndef OSDTEXTEDIT_H
#define OSDTEXTEDIT_H

#include <QColor>
#include <QFile>
#include <QImage>
#include <QTextEdit>

class OSDTextEdit : public QTextEdit
{
    Q_OBJECT

private:
    enum OSDTextEditFlagType
    {
        SmallImageModeFlag=0,
        UpdateBottomBlankAreaFlag,
        FileModifiedFlag
    };

    struct ImageSize
    {
        int width;
        int height;
    };

    enum FontAttributeType
    {
        FONT_ATTR_START=0,

        FONT_BOLD=1,
        FONT_ITALIC,
        FONT_UNDERLINE,
        FONT_DELETELINE,
        FONT_SIZE,
        FONT_FAMILY,

        FONT_ATTR_END,
    };

public:
    OSDTextEdit(QWidget *parent);

    void insertImage(const QImage& img);

    void setOSD(const QString& osd);
    QString toOSD();
    bool isFontBold() const;
    bool isFontItalic() const;
    bool isFontUnderline() const;
    bool isFontDeleteline() const;
    QColor fontForegroundColor() const;
    QColor fontBackgroundColor() const;
    QString fontFamily() const;
    int fontSize() const;
    bool isSmallImageMode() const;
    void resetFontFormat();
    void setFontAttributes(FontAttributeType type, QVariant value);
    bool openFromFile(const QString &fileName);
    bool saveToFile(const QString &fileName);
    bool saveToFile();
    bool isFileModified() const;
    bool isFileExist() const;
    QString fileName() const;
    bool printPDF(const QString fileName);
    void createTbl();
    void createList(const QTextListFormat::Style &style);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void resizeEvent(QResizeEvent *e) override;
    bool tabOperation(bool isAdd);
    void updateBottomBlankArea();
    bool jumpTabCell(bool isToBack);
    bool setTabCellHAlignment();
    bool setTabCellVAlignment();
    bool listTabOperation(bool isToRight);
    bool tableIndentOperation(bool isToRight);

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
    void on_cursorPositionChanged();
    void on_selectionChanged();
    void on_textChanged();
    void on_customContextMenuRequested(const QPoint& point);
    void on_saveImage();
    void on_copyImage();
    void on_insertRow();
    void on_appendRow();
    void on_removeRow();
    void on_insertColumn();
    void on_appendColumn();
    void on_removeColumn();

private:
    bool testFlag(OSDTextEditFlagType type) const;
    void setFlag(OSDTextEditFlagType type, bool value);
    void travelToResizeImage(QTextFrame *frame, bool isSmall,
                             int height, int width);
    bool canInsertFromMimeData(const QMimeData *source) const;
    void insertFromMimeData(const QMimeData *source);

private:
    quint32 mFlags;
    int mMaxImageHeight;
    int mMaxImageWidth;
    QFile mCurFile;
    QString mClickedImgName;
    QMap<QString, ImageSize> mImageOriginSizeMap;
    QMap<QString, QList<ExtraSelection>> mExtraSelections;
};

#endif // OSDTEXTEDIT_H

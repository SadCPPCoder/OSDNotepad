/*******************************************
 * File Name: mainwindow.cpp
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Implementation for the main
 * window for the OSD Notepad application.
 *******************************************/

#include <QApplication>
#include <QCoreApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QHBoxLayout>
#include <QImageReader>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QMessageBox>
#include <QMimeData>
#include <QMouseEvent>
#include <QPrinter>
#include <QPushButton>
#include <QScrollBar>
#include <QTextLayout>
#include <QTextBlock>
#include <QToolButton>
#include "mainwindow.h"
#include "globalinfo.h"
#include "osdconfig.h"
#include "ui_mainwindow.h"

static const int DefaultFontSize = 14;
static const QString tempFileName("./temp.osd");
static const int DefaultMinImgHeight = 100;

MainWindow::MainWindow(QString file, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mFontCobox(new OSDFontComboBox(this)),
    mFontSizeSpinbox(new OSDSpinBox(this)),
    mForeColorCobox(new OSDColorComboBox(this, OSDColorComboBox::OSDBlack)),
    mBackColorCobox(new OSDColorComboBox(this, OSDColorComboBox::OSDWhite)),
    mTextEdit(NULL),
    mImgMinSize(DefaultMinImgHeight),
    mPositionLabel(new QLabel(tr("Col: ") + "0, " + tr("Row: ") + "0", this)),
    mWordCntLabel(new QLabel(tr("Char: ") + "0", this)),
    mFilePathEdit(new QLineEdit(tr("New File"), this)),
    mSaveStatusLabel(new QLabel(tr("Unsaved"), this)),
    mAbout(new About(this)),
    mTabWgt(new QTabWidget(this)),
    mRecentListWgt(new QListWidget(this)),
    mMousePoint(0, 0),
    mMousePressed(false),
    mSearchBar(this),
    mShortcutDlg(new OSDShortcutDialog(this))
{
    ui->setupUi(this);
    mSearchBar.hide();
    mAbout->setModal(true);
    mShortcutDlg->setModal(true);
    QRect screenRect = QApplication::desktop()->screenGeometry();
    QRect windowRect(screenRect.width()*0.2, screenRect.height()*0.2,
                     screenRect.width()*0.6, screenRect.height()*0.6);
    setGeometry(windowRect);

    // set a widget as central widget
    // add text edit to the widget and
    // set the layout to set the boarder
    // off view mode.
    QWidget *widget = new QWidget(this);
    setCentralWidget(widget);
    QVBoxLayout *layout = new QVBoxLayout(widget);
    layout->addWidget(mTabWgt);
    layout->addWidget(&mSearchBar);
    layout->setMargin(0);
    layout->setSpacing(0);
    widget->setLayout(layout);
    QPalette plt = widget->palette();
    widget->setAutoFillBackground(true);
    widget->setPalette(plt);

    setupExtraUi();
    auto title = windowTitle();
    title += QString::asprintf("_%d.%d", MAIN_VERSION_NO, SUB_VERSION_NO);
    setWindowTitle(title);

    QFileInfo fInfo(file);
    if(fInfo.exists())
    {
        addNewEditPage(file);
    }
    else
    {
        openLastFile();
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event);
    mMousePressed = false;
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->button() == Qt::LeftButton)
    {
        mMousePressed = true;
        mMousePoint = event->pos();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if(mMousePressed)
    {
        mMousePressed = true;
        move(event->pos() - mMousePoint + pos());
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    OSDConfig::getInstance().cleanUnclosedFile();
    for(int i = mTabWgt->count() - 1; i >= 0 ; --i)
    {
        auto edit = qobject_cast<OSDTextEdit*>(mTabWgt->widget(i));
        auto fileName = closeEditPage(edit, i);
        if(!fileName.isEmpty())
        {
            OSDConfig::getInstance().appendUnclosedFile(fileName);
        }
    }

    OSDConfig::getInstance().updateConfigFile();
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    QKeySequence keySeq(event->modifiers() + event->key());
    if(keySeq.matches(OSDConfig::getInstance()
                      .getKeySequence(SK_FontSizeIncrease)))
    {
        mFontSizeSpinbox->stepUp();
        mTextEdit->setFocus();
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                        .getKeySequence(SK_FontSizeDecrease)))
    {
        mFontSizeSpinbox->stepDown();
        mTextEdit->setFocus();
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                        .getKeySequence(SK_SearchBarShow)))
    {
        mSearchBar.show();
        mSearchBar.setFocus();
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                        .getKeySequence(SK_SearchBarHide)))
    {
        mSearchBar.hide();
        mTextEdit->setFocus();
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                        .getKeySequence(SK_FormatCopy)))
    {
        copyCharFormat();
    }
    else if(keySeq.matches(OSDConfig::getInstance()
                        .getKeySequence(SK_FormatPaste)))
    {
        pasteCharFormat();
    }
}

void MainWindow::dragEnterEvent(QDragEnterEvent *event)
{
    if(event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();
    if(mimeData->hasUrls())
    {
        QList<QUrl> urls = mimeData->urls();
        for(int i=0; i<urls.size(); ++i)
        {
            QString fileName = urls.at(i).toLocalFile();
            QFileInfo fileInfo(fileName);
            if(fileInfo.exists() &&
                (!fileInfo.suffix().compare("osd", Qt::CaseInsensitive) ||
                 !fileInfo.suffix().compare("txt", Qt::CaseInsensitive)))
            {
                OSDConfig::getInstance().appendRecentFile(fileName);
                OSDConfig::getInstance().updateLastOpenedDir(fileInfo.absolutePath());
                addNewEditPage(fileName);
            }
        }
    }
}

void MainWindow::setupExtraUi()
{
    mTabWgt->setMovable(true);
    mTabWgt->setTabsClosable(true);
    mTabWgt->setElideMode(Qt::ElideLeft);

    connect(mTabWgt, SIGNAL(currentChanged(int)),
            this, SLOT(on_currentChanged(int)));
    connect(mTabWgt, SIGNAL(tabCloseRequested(int)),
            this, SLOT(on_tabCloseRequested(int)));

    on_actionReset_triggered();

    mFontCobox->setSizeAdjustPolicy(
                QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
    ui->toolBarFont->insertWidget(ui->actionReset, new QLabel("  ", this));
    ui->toolBarFont->insertWidget(ui->actionReset, mFontCobox);
    connect(mFontCobox, SIGNAL(currentFontFamilyChanged(QString)),
            this, SLOT(fontFamilyChanged(QString)));
    mFontCobox->setToolTip(tr("Select font."));
    //mFontCobox->view()->setTextElideMode(Qt::ElideNone);
    mFontCobox->view()->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    mFontCobox->view()->horizontalScrollBar()->setEnabled(true);

    ui->toolBarFont->insertWidget(ui->actionReset, new QLabel("  ", this));
    ui->toolBarFont->insertWidget(ui->actionReset, mFontSizeSpinbox);
    connect(mFontSizeSpinbox, SIGNAL(currentValueChanged(int)),
            this, SLOT(fontSizeChanged(int)));
    mFontSizeSpinbox->setToolTip(
                tr("Set font size, Ctr+- to decrease, Ctrl+= to increase."));

    ui->toolBarFont->insertSeparator(ui->actionReset);
    QLabel *foregroundColorLbl = new QLabel(this);
    foregroundColorLbl->setMargin(5);
    foregroundColorLbl->setToolTip(tr("Text color."));
    QImage imgFore(":/res/icon/foreground_color.png");
    QPixmap pixFore = QPixmap::fromImage(imgFore);
    foregroundColorLbl->setPixmap(pixFore.scaled(20, 20,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    ui->toolBarFont->insertWidget(ui->actionReset, foregroundColorLbl);
    ui->toolBarFont->insertWidget(ui->actionReset, mForeColorCobox);
    connect(mForeColorCobox, SIGNAL(currentColorChanged(QColor)),
            this, SLOT(foregroundColorChanged(QColor)));
    mForeColorCobox->setToolTip(tr("Set text color."));

    QLabel *backgroundColorLbl = new QLabel(this);
    backgroundColorLbl->setMargin(5);
    backgroundColorLbl->setToolTip(tr("Background color."));
    QImage imgBack(":/res/icon/background_color.png");
    QPixmap pixBack = QPixmap::fromImage(imgBack);
    backgroundColorLbl->setPixmap(pixBack.scaled(20, 20,
                Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
    ui->toolBarFont->insertWidget(ui->actionReset, backgroundColorLbl);
    ui->toolBarFont->insertWidget(ui->actionReset, mBackColorCobox);
    connect(mBackColorCobox, SIGNAL(currentColorChanged(QColor)),
            this, SLOT(backgroundColorChanged(QColor)));
    mBackColorCobox->setToolTip(tr("Set background color."));

    mFilePathEdit->setAlignment(Qt::AlignRight);
    mFilePathEdit->setReadOnly(true);
    mSaveStatusLabel->setFrameShape(QFrame::StyledPanel);
    mPositionLabel->setFrameShape(QFrame::StyledPanel);
    mWordCntLabel->setFrameShape(QFrame::StyledPanel);
    ui->statusBar->addWidget(mFilePathEdit);
    ui->statusBar->addWidget(mSaveStatusLabel);

    QToolButton* top = new QToolButton(this);
    top->setIconSize({16, 16});
    top->setBaseSize(16, 16);
    top->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    top->setDefaultAction(ui->actionTop);
    ui->statusBar->addPermanentWidget(top);

    QToolButton* view = new QToolButton(this);
    view->setDefaultAction(ui->actionView);
    view->setIconSize({16, 16});
    view->setBaseSize(16, 16);
    view->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    ui->statusBar->addPermanentWidget(view);

    ui->statusBar->addPermanentWidget(mPositionLabel);
    ui->statusBar->addPermanentWidget(mWordCntLabel);

    QPalette plt = mSearchBar.palette();
    plt.setColor(QPalette::Background, Qt::white);
    mSearchBar.setAutoFillBackground(true);
    mSearchBar.setPalette(plt);

    mRecentListWgt->setTextElideMode(Qt::ElideMiddle);
    mRecentListWgt->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    connect(mRecentListWgt, SIGNAL(itemDoubleClicked(QListWidgetItem*)),
            this, SLOT(on_itemDoubleClicked(QListWidgetItem*)));
    mRecentListWgt->hide();
    this->installEventFilter(this);
    mRecentListWgt->installEventFilter(this);
}

bool MainWindow::addNewEditPage(const QString &fileName)
{
    static int newFileIndex = 0;

    QString fileFullName = fileName.isEmpty() ?
                        QString::asprintf("NewFile_%d", newFileIndex++) :
                        fileName;

    for(int i = 0; i < mTabWgt->count(); ++i)
    {
        if(fileFullName == mTabWgt->tabText(i))
        {
            mTabWgt->setCurrentIndex(i);
            return true;
        }
    }

    mTextEdit = new OSDTextEdit(this);

    // init edit
    mTextEdit->setFontFamily(mFontCobox->currentText());

    connect(mTextEdit, SIGNAL(cursorPositionChanged()),
            this, SLOT(textEditCursorPositionChanged()));

    connect(mTextEdit, SIGNAL(textChanged()),
            this, SLOT(textEditTextChanged()));

    if(!fileName.isEmpty())
    {
        if(!mTextEdit->openFromFile(fileFullName))
            return false;
    }
    int tabId = mTabWgt->addTab(mTextEdit, fileFullName);
    if(tabId >=0 )
    {
        mTabWgt->setCurrentIndex(tabId);
    }

    return true;
}

void MainWindow::fontFamilyChanged(const QString& family)
{
    mTextEdit->setFontFamily(family);
}

void MainWindow::fontSizeChanged(int size)
{
    mTextEdit->setFontSize(size);
}

void MainWindow::foregroundColorChanged(const QColor &color)
{
    mTextEdit->setFontForegroundColor(color);
}

void MainWindow::backgroundColorChanged(const QColor &color)
{
    mTextEdit->setFontBackgroundColor(color);
}

void MainWindow::textEditCursorPositionChanged()
{
    OSDTextEdit *senderEdit = qobject_cast<OSDTextEdit*>(sender());
    if(mTextEdit == senderEdit)
    {
        updateCharRelatedUI();
    }
}

void MainWindow::textEditTextChanged()
{
    OSDTextEdit *senderEdit = qobject_cast<OSDTextEdit*>(sender());
    if(mTextEdit == senderEdit)
    {
        // status changed
        updateFileRelatedUI();
    }
}

void MainWindow::openLastFile()
{
    if(OSDConfig::getInstance().getUnclosedFileList().isEmpty())
    {
        addNewEditPage("");
        return;
    }

    auto itr = OSDConfig::getInstance().getUnclosedFileList().rbegin();

    for(; itr != OSDConfig::getInstance().getUnclosedFileList().rend(); ++itr)
    {
        QFileInfo fileInfo(*itr);
        if(fileInfo.exists())
        {
            addNewEditPage(*itr);
        }
    }
}

bool MainWindow::fileFormatMatch(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    if(fileInfo.suffix() != "osd" && fileInfo.suffix() != "txt")
    {
        QMessageBox::warning(this, tr("Notice"),
            tr("Wrong file format, file should be *.osd."));
        return false;
    }

    return true;
}

void MainWindow::updateFileRelatedUI()
{
    // status changed
    mWordCntLabel->setText(tr("Char: ")+QString::number(mTextEdit->document()->characterCount()-1));
    mSaveStatusLabel->setText(mTextEdit->isFileModified() ? tr("Unsaved") : tr("Saved"));

    setFilePath(mTabWgt->tabText(mTabWgt->currentIndex()));
    ui->actionSmallImage->setChecked(mTextEdit->isSmallImageMode());
    mSearchBar.setTextEdit(mTextEdit);
}

void MainWindow::updateCharRelatedUI()
{
    QTextCursor cursor = mTextEdit->textCursor();
    auto charFormat = cursor.charFormat();

    QColor foreColor = Qt::black;
    if(charFormat.hasProperty(QTextFormat::ForegroundBrush))
    {
        foreColor = mTextEdit->fontForegroundColor();
    }

    QColor backColor = Qt::white;
    if(charFormat.hasProperty(QTextFormat::BackgroundBrush))
    {
        backColor = mTextEdit->fontBackgroundColor();
    }

    mForeColorCobox->setCurrentColor(foreColor);
    mBackColorCobox->setCurrentColor(backColor);
    mFontCobox->setCurrentFontFamily(mTextEdit->fontFamily());
    mFontSizeSpinbox->setCurrentValue(mTextEdit->fontSize());
    ui->actionBold->setChecked(mTextEdit->isFontBold());
    ui->actionItalic->setChecked(mTextEdit->isFontItalic());
    ui->actionUnderline->setChecked(mTextEdit->isFontUnderline());
    ui->actionDeleteline->setChecked(mTextEdit->isFontDeleteline());

    // update status bar
    mPositionLabel->setText(tr("Row: ")+QString::number(cursor.blockNumber()+1)+", "+
                            tr("Col: ")+QString::number(cursor.columnNumber()+1));
}

QString MainWindow::closeEditPage(OSDTextEdit *edit, const int index)
{
    if(!edit || index < 0)
        return "";

    QString savedFileName;

    // save file
    if(edit->isFileModified())
    {
        if(!edit->isFileExist())
        {
            auto btn = QMessageBox::warning(this, tr("Warning"),
                       tr("File \"") + mTabWgt->tabText(index) +
                       tr("\" does not exist, do you want to create it? "),
                       QMessageBox::Ok | QMessageBox::Discard, QMessageBox::Ok);
            if(QMessageBox::Ok == btn)
            {
                savedFileName = saveFile(edit, false);
            }
        }
        else
        {
            savedFileName = saveFile(edit, false);
        }
    }
    else
    {
        savedFileName = edit->fileName();
    }

    // remove tab
    mTabWgt->removeTab(index);

    return savedFileName;
}

void MainWindow::copyCharFormat()
{
    if(mTextEdit->textCursor().hasSelection())
    {
        // Use first character format
        auto cursor = mTextEdit->textCursor();
        cursor.setPosition(mTextEdit->textCursor().selectionStart() + 1);

        while(cursor.charFormat().toImageFormat().isValid()
              && cursor.position() != 0)
        {
            cursor.setPosition(cursor.position()-1);
        }

        mCopiedFont = cursor.charFormat().font();
        if(cursor.position())
        {
            mCopiedTextClr = cursor.charFormat().foreground().color();
            mCopiedBackClr = cursor.charFormat().background().color();
        }
        else
        {
            mCopiedTextClr = Qt::black;
            mCopiedBackClr = Qt::white;
        }
    }
    else
    {
        mCopiedFont = mTextEdit->currentFont();
        mCopiedTextClr = mTextEdit->textColor();
        mCopiedBackClr = mTextEdit->textBackgroundColor();
    }
}

void MainWindow::pasteCharFormat()
{
    mTextEdit->setCurrentFont(mCopiedFont);
    mTextEdit->setTextColor(mCopiedTextClr);
    mTextEdit->setTextBackgroundColor(mCopiedBackClr);
}

QString MainWindow::saveFile(OSDTextEdit *edit, bool saveAs)
{
    QString savedFileName;

    if(edit->isFileModified() || saveAs)
    {
        if(!edit->isFileExist() || saveAs)
        {
            QString curPath = OSDConfig::getInstance().getLastOpenedDir();
            if(curPath.isEmpty())
            {
                curPath = QCoreApplication::applicationDirPath();
            }
            QString dlgTitle(tr("Save File"));
            QString filter=tr("OSD File(*.osd)");
            if(saveAs)
            {
                filter += tr(";;PDF File(*.pdf)");
            }
            QString selectedFilter;
            QString fileName = QFileDialog::getSaveFileName(this,
                                        dlgTitle, curPath,
                                        filter, &selectedFilter);
            if(!fileName.isEmpty())
            {
                auto suffix = QFileInfo(fileName).suffix();
                if(suffix.isEmpty())
                {
                    suffix = "osd";
                    fileName += ".osd";
                }

                if(!suffix.compare("pdf", Qt::CaseInsensitive))
                {
                    edit->printPDF(fileName);
                }
                else
                {
                    edit->saveToFile(fileName);
                    OSDConfig::getInstance().appendRecentFile(fileName);
                    savedFileName = fileName;
                }
            }
        }
        else
        {
            edit->saveToFile();
            savedFileName = edit->fileName();
        }
    }

    return savedFileName;
}

QString MainWindow::openFile()
{
    QString openedFile;
    QString curPath = OSDConfig::getInstance().getLastOpenedDir();
    if(curPath.isEmpty())
    {
        curPath = QCoreApplication::applicationDirPath();
    }
    QString dlgTitle(tr("Open File"));
    QString filter=tr("OSD file(*.osd);;Text file(*.txt)");
    QString fileName = QFileDialog::getOpenFileName(this,
                                dlgTitle, curPath, filter);

    QFileInfo fileInfo(fileName);
    if(fileInfo.exists())
    {
        OSDConfig::getInstance().appendRecentFile(fileName);
        OSDConfig::getInstance().updateLastOpenedDir(fileInfo.absolutePath());
        addNewEditPage(fileName);
        openedFile = openedFile;
    }

    return openedFile;
}

void MainWindow::setFilePath(const QString &path)
{
    auto width = mFilePathEdit->fontMetrics().width(path) + 10;
    if(width > 500)
    {
        width = 500;
    }
    mFilePathEdit->setText(path);
    mFilePathEdit->setMaximumWidth(width);
}

void MainWindow::on_actionReset_triggered()
{
    // Reset UI
    mFontSizeSpinbox->setCurrentValue(DefaultFontSize);
    mForeColorCobox->setCurrentColor(Qt::black);
    mBackColorCobox->setCurrentColor(Qt::white);
    ui->actionBold->setChecked(false);
    ui->actionItalic->setChecked(false);
    ui->actionUnderline->setChecked(false);
    ui->actionDeleteline->setChecked(false);

    // Reset Font
    if(mTextEdit)
    {
        mFontCobox->setCurrentFontFamily(mTextEdit->currentFont().defaultFamily());
        mTextEdit->resetFontFormat();
    }
}

void MainWindow::on_actionBold_triggered(bool checked)
{
    mTextEdit->setFontBold(checked);
}

void MainWindow::on_actionItalic_triggered(bool checked)
{
    mTextEdit->setFontItalic(checked);
}

void MainWindow::on_actionUnderline_triggered(bool checked)
{
    mTextEdit->setFontUnderline(checked);
}

void MainWindow::on_actionDeleteline_triggered(bool checked)
{
    mTextEdit->setFontDeleteline(checked);
}

void MainWindow::on_actionOpen_triggered()
{
    openFile();
}

void MainWindow::on_actionSave_triggered()
{
    auto fileName = saveFile(mTextEdit, false);

    if(!fileName.isEmpty())
    {
        mTabWgt->setTabText(mTabWgt->currentIndex(), fileName);
        updateFileRelatedUI();
    }
}

void MainWindow::on_actionInsertImage_triggered()
{
    QString curPath = OSDConfig::getInstance().getLastOpenedImgDir();
    if(curPath.isEmpty())
    {
        curPath = QCoreApplication::applicationDirPath();
    }

    QString file = QFileDialog::getOpenFileName(this, tr("Select an image"),
                curPath, tr("all (*.*)\n"
                "JPEG (*.jpg *jpeg)\n"
                "GIF (*.gif)\n"
                "PNG (*.png)\n"));

    if(!file.isEmpty())
    {
        QUrl Uri ( file );
        QImage image = QImageReader ( file ).read();
        mTextEdit->insertImage(image);

        QFileInfo fileInfo(file);
        OSDConfig::getInstance().updateLastOpenedImgDir(fileInfo.absolutePath());
    }
}

void MainWindow::on_actionNew_triggered()
{
    addNewEditPage("");
}

void MainWindow::on_actionSmallImage_triggered(bool checked)
{
    mTextEdit->setSmallImageMode(checked ? mImgMinSize : 0);
}

void MainWindow::on_actionTop_triggered(bool checked)
{
    if(checked)
    {
        setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    }
    else
    {
        setWindowFlags(windowFlags() & (~Qt::WindowStaysOnTopHint));
    }
    showNormal();
}

void MainWindow::on_actionView_triggered(bool checked)
{
    if(checked)
    {
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
        centralWidget()->layout()->setMargin(4);
    }
    else
    {
        setWindowFlags(windowFlags() & (~Qt::FramelessWindowHint));
        centralWidget()->layout()->setMargin(0);
    }
    showNormal();

    static int defaultStatusH = ui->statusBar->height();
    bool visible = !checked;
    ui->toolBarFile->setVisible(visible);
    ui->toolBarFont->setVisible(visible);

    for(int i=0; i<mTabWgt->count(); ++i)
    {
        auto edit = qobject_cast<OSDTextEdit*>(mTabWgt->widget(i));
        edit->setVerticalScrollBarPolicy(
                    visible ? Qt::ScrollBarAsNeeded:
                              Qt::ScrollBarAlwaysOff);
        edit->setHorizontalScrollBarPolicy(
                    visible ? Qt::ScrollBarAsNeeded:
                              Qt::ScrollBarAlwaysOff);
        edit->setReadOnly(checked);
    }

    ui->statusBar->setMaximumHeight(visible ? defaultStatusH : 18);
    ui->statusBar->adjustSize();

}

void MainWindow::on_actionTransIncrease_triggered()
{
    if(windowOpacity() < 1.0)
    {
        setWindowOpacity(windowOpacity() + 0.05);
    }
}

void MainWindow::on_actionTransDecrease_triggered()
{
    if(windowOpacity() > 0.1)
    {
        setWindowOpacity(windowOpacity() - 0.05);
    }
}

void MainWindow::on_actionAbout_triggered()
{
    mAbout->setModal(true);
    mAbout->show();
}

void MainWindow::on_currentChanged(int index)
{
    auto widget = mTabWgt->widget(index);
    if(widget)
    {
        mTextEdit = qobject_cast<OSDTextEdit*>(widget);
        updateFileRelatedUI();
        updateCharRelatedUI();
    }
}

void MainWindow::on_tabCloseRequested(int index)
{
    auto edit = qobject_cast<OSDTextEdit *>(mTabWgt->widget(index));
    closeEditPage(edit, index);

    if(mTabWgt->count() == 0)
    {
        this->close();
    }
}

void MainWindow::on_actionOpenRecent_triggered()
{
    if(mRecentListWgt->isVisible())
    {
        mRecentListWgt->hide();
        return;
    }

    auto actWgt = ui->toolBarFile->widgetForAction(ui->actionOpenRecent);
    if(actWgt)
    {
        const int len = 150;
        mRecentListWgt->clear();
        mRecentListWgt->addItems(OSDConfig::getInstance().getRecentFileList());

        for(int i=0; i<mRecentListWgt->count(); ++i)
        {
            mRecentListWgt->item(i)->setToolTip(mRecentListWgt->item(i)->text());
        }

        mRecentListWgt->raise();
        int tempX = ui->toolBarFile->orientation() == Qt::Vertical ?
                    ui->toolBarFile->x() + actWgt->x() + len >= width() ?
                        ui->toolBarFile->x() - len :
                        ui->toolBarFile->x() + ui->toolBarFile->width() :
                    ui->toolBarFile->x() + actWgt->x();

        int tempY = ui->toolBarFile->orientation() == Qt::Vertical ?
                    (ui->toolBarFile->y() + actWgt->y() + len >= height() ?
                        height() - len :
                        ui->toolBarFile->y() + actWgt->y()):
                    (ui->toolBarFile->y() + actWgt->y() + len >= height() ?
                        ui->toolBarFile->y() - len :
                        actWgt->y() + actWgt->height() + ui->toolBarFile->y());

        mRecentListWgt->setGeometry(tempX, tempY, len, len);
        mRecentListWgt->show();
        mRecentListWgt->setFocus();
    }
}

void MainWindow::on_itemDoubleClicked(QListWidgetItem *item)
{
    mRecentListWgt->hide();

    QString fileName = item->text();
    QFileInfo fileInfo(fileName);
    if(fileInfo.exists())
    {
        OSDConfig::getInstance().appendRecentFile(fileName);
        OSDConfig::getInstance().updateLastOpenedDir(fileInfo.absolutePath());
        addNewEditPage(fileName);
    }
    else
    {
        mRecentListWgt->removeItemWidget(item);
        OSDConfig::getInstance().removeRecentFile(fileName);
    }
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == mRecentListWgt)
    {
        if(event->type() == QEvent::Leave ||
           event->type() == QEvent::FocusOut)
        {
            mRecentListWgt->hide();
            return true;
        }
    }
    else
    {
        if(event->type() == QEvent::NonClientAreaMouseButtonPress)
        {
            if(mRecentListWgt->isVisible())
            {
                mRecentListWgt->hide();
                return true;
            }
        }
    }

    return QWidget::eventFilter(watched,event);
}

void MainWindow::on_actionSaveAs_triggered()
{
    saveFile(mTextEdit, true);
}

void MainWindow::on_actionShortcut_triggered()
{
    mShortcutDlg->show();
}

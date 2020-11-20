#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QCoreApplication>
#include <QFileDialog>
#include <QImageReader>
#include <QLabel>
#include <QMenu>
#include <QMessageBox>
#include <QLineEdit>
#include <QToolButton>
#include <QScrollBar>

static const int DefaultFontSize = 14;
static const QString tempFileName("./temp.osd");
static const QString configFileName("./config.txt");
static const QString lastFileConfig("LastOpenedFile=");
static const int DefaultMinImgHeight = 100;
static const QString colStr = QObject::tr("Col: ");
static const QString rowStr = QObject::tr("Row: ");
static const QString wordCntStr = QObject::tr("Word: ");
static const QString newFileStr = QObject::tr("New File");
static const QString changedStatusStr = QObject::tr("Unsaved");
static const QString savedStatusStr = QObject::tr("Saved");

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    mFontCobox(new OSDFontComboBox(this)),
    mFontSizeSpinbox(new OSDSpinBox(this)),
    mForeColorCobox(new OSDColorComboBox(this, OSDColorComboBox::OSDBlack)),
    mBackColorCobox(new OSDColorComboBox(this, OSDColorComboBox::OSDWhite)),
    mTextEdit(new OSDTextEdit(this)),
    mCurFile(),
    mFileModifiedFlag(false),
    mImgMinSize(DefaultMinImgHeight),
    mPositionLabel(new QLabel(colStr + "0, " + rowStr + "0", this)),
    mWordCntLabel(new QLabel(wordCntStr + "0", this)),
    mFilePathEdit(new QLineEdit(newFileStr, this)),
    mSaveStatusLabel(new QLabel(changedStatusStr, this))
{
    ui->setupUi(this);
    setupExtraUi();

    setCentralWidget(mTextEdit);

    openLastFile();

    ui->actionSmallImage->setChecked(mTextEdit->isSmallImageMode());
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event);
    if(!mCurFile.exists() &&
       mFileModifiedFlag &&
       !mTextEdit->toPlainText().isEmpty())
    {
        QFile file(tempFileName);
        file.open(QIODevice::WriteOnly);

        file.write(mTextEdit->toOSD().toUtf8());

        file.close();
    }
    else if(mCurFile.exists())
    {
        if(mFileModifiedFlag)
        {
            on_actionSave_triggered();
        }

        QFile file(configFileName);
        file.open(QIODevice::WriteOnly);
        QString lastOpenedFile = lastFileConfig;
        lastOpenedFile.append(mCurFile.fileName());
        file.write(lastOpenedFile.toUtf8());
        file.close();
    }
}

void MainWindow::setupExtraUi()
{
    mFontCobox->setSizeAdjustPolicy(
                QComboBox::SizeAdjustPolicy::AdjustToMinimumContentsLength);
    ui->toolBarFont->insertWidget(ui->actionReset, new QLabel("  ", this));
    ui->toolBarFont->insertWidget(ui->actionReset, mFontCobox);
    connect(mFontCobox, SIGNAL(currentFontFamilyChanged(QString)),
            this, SLOT(fontFamilyChanged(QString)));

    mTextEdit->setFontFamily(mFontCobox->currentText());

    ui->toolBarFont->insertWidget(ui->actionReset, new QLabel("  ", this));
    ui->toolBarFont->insertWidget(ui->actionReset, mFontSizeSpinbox);
    connect(mFontSizeSpinbox, SIGNAL(currentValueChanged(int)),
            this, SLOT(fontSizeChanged(int)));

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

    connect(mTextEdit, SIGNAL(cursorPositionChanged()),
            this, SLOT(textEditCursorPositionChanged()));

    connect(mTextEdit, SIGNAL(textChanged()),
            this, SLOT(textEditTextChanged()));

    mTextEdit->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(mTextEdit, SIGNAL(customContextMenuRequested(QPoint)),
            this, SLOT(textEditContentMenu(QPoint)));

    mFilePathEdit->setAlignment(Qt::AlignRight);
    mFilePathEdit->setReadOnly(true);
    setFilePath(newFileStr);
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
    mPositionLabel->setText(rowStr+QString::number(cursor.blockNumber()+1)+", "+
                            colStr+QString::number(cursor.columnNumber()+1));
}

void MainWindow::textEditTextChanged()
{
    mFileModifiedFlag = true;

    // status changed
    mWordCntLabel->setText(wordCntStr+QString::number(mTextEdit->toPlainText().length()));
    mSaveStatusLabel->setText(changedStatusStr);
}

void MainWindow::openLastFile()
{
    // temp file
    QFile tmpFile(tempFileName);
    QFile configFile(configFileName);
    if(tmpFile.exists())
    {
        tmpFile.open(QIODevice::ReadOnly);
        mTextEdit->setOSD(tmpFile.readAll());
        tmpFile.close();
        tmpFile.remove();
        mFileModifiedFlag = true;
        setFilePath(newFileStr);
        mSaveStatusLabel->setText(changedStatusStr);
    }
    else if(configFile.exists())
    {
        configFile.open(QIODevice::ReadOnly);
        QString lastFile = configFile.readLine();
        configFile.close();

        if(lastFile.contains(lastFileConfig))
        {
            auto pos = lastFile.indexOf('=');
            if(pos != -1)
            {
                QString lastFileName(lastFile.midRef(pos+1).toString());

                if(!fileFormatMatch(lastFileName))
                {
                    return;
                }

                mCurFile.setFileName(lastFileName);
                mCurFile.open(QIODevice::ReadOnly);
                mTextEdit->setOSD(mCurFile.readAll());
                mCurFile.close();
                mFileModifiedFlag = false;
                setFilePath(lastFileName);
                mSaveStatusLabel->setText(savedStatusStr);
            }
        }
    }
}

bool MainWindow::fileFormatMatch(const QString& fileName)
{
    QFileInfo fileInfo(fileName);
    if(fileInfo.suffix() != "osd")
    {
        QMessageBox::warning(this, tr("Notice"),
            tr("Wrong file format, file should be *.osd."));
        return false;
    }

    return true;
}

void MainWindow::textEditContentMenu(const QPoint& point)
{
    Q_UNUSED(point);
    QMenu *menu = mTextEdit->createStandardContextMenu();
    menu->addAction(ui->actionView);
    menu->exec(QCursor::pos());
    delete menu;
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
    mFontCobox->setCurrentFontFamily(mTextEdit->currentFont().defaultFamily());
    mFontSizeSpinbox->setCurrentValue(DefaultFontSize);
    mForeColorCobox->setCurrentColor(Qt::black);
    mBackColorCobox->setCurrentColor(Qt::white);
    ui->actionBold->setChecked(false);
    ui->actionItalic->setChecked(false);
    ui->actionUnderline->setChecked(false);
    ui->actionDeleteline->setChecked(false);

    // Reset Font
    mTextEdit->setFontFamily(mTextEdit->currentFont().defaultFamily());
    mTextEdit->setFontSize(DefaultFontSize);
    mTextEdit->setFontForegroundColor(Qt::black);
    mTextEdit->setFontBackgroundColor(Qt::white);
    mTextEdit->setFontBold(false);
    mTextEdit->setFontItalic(false);
    mTextEdit->setFontUnderline(false);
    mTextEdit->setFontDeleteline(false);
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
    bool saveModifiedFlag = false;
    if(mFileModifiedFlag)
    {
        auto result = QMessageBox::question(this, tr("Save File"),
                 tr("File is modified, do you want to save?"),
                 QMessageBox::Yes|QMessageBox::No,
                 QMessageBox::Yes);
        if(QMessageBox::Yes == result)
        {
            on_actionSave_triggered();
            saveModifiedFlag = true;
        }
    }

    if(!saveModifiedFlag)
    {
        QString curPath = QCoreApplication::applicationDirPath();
        QString dlgTitle(tr("Open File"));
        QString filter=tr("OSD file(*.osd)");
        QString fileName = QFileDialog::getOpenFileName(this,
                                    dlgTitle, curPath, filter);
        if(!fileName.isEmpty())
        {
            QFileInfo fileInfo(fileName);
            mCurFile.setFileName(fileName);
            mCurFile.open(QIODevice::ReadOnly);

            mTextEdit->setOSD(mCurFile.readAll());

            mCurFile.close();
            mFileModifiedFlag = false;

            setFilePath(fileName);
            mSaveStatusLabel->setText(savedStatusStr);
        }
    }
}

void MainWindow::on_actionSave_triggered()
{
    if(mFileModifiedFlag)
    {
        if(!mCurFile.exists())
        {
            QString curPath = QCoreApplication::applicationDirPath();
            QString dlgTitle(tr("Save File"));
            QString filter=tr("OSD File(*.osd)");
            QString selectedFilter;
            QString fileName = QFileDialog::getSaveFileName(this,
                                        dlgTitle, curPath,
                                        filter, &selectedFilter);
            if(!fileName.isEmpty())
            {
                QFileInfo fileInfo(fileName);
                auto fileSuffix = fileInfo.suffix();
                if(fileSuffix.isEmpty())
                {
                    fileSuffix = ".osd";
                    fileName.append(fileSuffix);
                }

                QFile file(fileName);
                file.open(QIODevice::WriteOnly);
                file.write(mTextEdit->toOSD().toLocal8Bit());
                file.close();
            }
        }
        else
        {
            QFileInfo fileInfo(mCurFile.fileName());
            mCurFile.open(QIODevice::WriteOnly);

            mCurFile.write(mTextEdit->toOSD().toUtf8());

            mCurFile.close();
        }
        mFileModifiedFlag = false;
        mSaveStatusLabel->setText(savedStatusStr);
    }
}

void MainWindow::on_actionInsertImage_triggered()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Select an image"),
                ".", tr("all (*.*)\n"
                "JPEG (*.jpg *jpeg)\n"
                "GIF (*.gif)\n"
                "PNG (*.png)\n"));
    if(!file.isEmpty())
    {
        QUrl Uri ( file );
        QImage image = QImageReader ( file ).read();
        mTextEdit->insertImage(image);
    }
}

void MainWindow::on_actionNew_triggered()
{
    if(mFileModifiedFlag)
    {
        auto result = QMessageBox::question(this, tr("Save File"),
                 tr("File is modified, do you want to save?"),
                 QMessageBox::Yes|QMessageBox::No,
                 QMessageBox::Yes);
        if(QMessageBox::Yes == result)
        {
            on_actionSave_triggered();
        }
    }

    mCurFile.setFileName("");
    mTextEdit->clear();
    mFileModifiedFlag = false;
    setFilePath(newFileStr);
    mSaveStatusLabel->setText(changedStatusStr);
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
    }
    else
    {
        setWindowFlags(windowFlags() & (~Qt::FramelessWindowHint));
    }
    showNormal();

    static int defaultStatusH = 0;
    bool visible = !checked;
    ui->toolBarFile->setVisible(visible);
    ui->toolBarFont->setVisible(visible);
    mTextEdit->setVerticalScrollBarPolicy(
                visible ? Qt::ScrollBarAsNeeded:
                          Qt::ScrollBarAlwaysOff);
    mTextEdit->setHorizontalScrollBarPolicy(
                visible ? Qt::ScrollBarAsNeeded:
                          Qt::ScrollBarAlwaysOff);

    if(!defaultStatusH)
    {
        defaultStatusH = ui->statusBar->height();
    }
    ui->statusBar->setMaximumHeight(visible ? defaultStatusH : 1);

    mTextEdit->setReadOnly(checked);
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontComboBox>
#include <QFile>
#include <QLabel>
#include "osdtextedit.h"
#include "osdcolorcombobox.h"
#include "osdfontcombobox.h"
#include "osdspinbox.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private:
    void setupExtraUi();

private slots:
    void fontFamilyChanged(const QString& family);
    void fontSizeChanged(int size);
    void foregroundColorChanged(const QColor& color);
    void backgroundColorChanged(const QColor& color);
    void textEditCursorPositionChanged();
    void textEditTextChanged();
    void openLastFile();
    bool fileFormatMatch(const QString& fileName);
    void textEditContentMenu(const QPoint& point);
    void setFilePath(const QString& path);


    void on_actionReset_triggered();

    void on_actionBold_triggered(bool checked);

    void on_actionItalic_triggered(bool checked);

    void on_actionUnderline_triggered(bool checked);

    void on_actionDeleteline_triggered(bool checked);

    void on_actionOpen_triggered();

    void on_actionSave_triggered();

    void on_actionInsertImage_triggered();

    void on_actionNew_triggered();

    void on_actionSmallImage_triggered(bool checked);

    void on_actionTop_triggered(bool checked);

    void on_actionView_triggered(bool checked);

    void on_actionTransIncrease_triggered();

    void on_actionTransDecrease_triggered();

private:
    Ui::MainWindow *ui;
    OSDFontComboBox *mFontCobox;
    OSDSpinBox *mFontSizeSpinbox;
    OSDColorComboBox *mForeColorCobox;
    OSDColorComboBox *mBackColorCobox;
    OSDTextEdit *mTextEdit;
    QFile mCurFile;
    bool mFileModifiedFlag;
    int mImgMinSize;
    QLabel *mPositionLabel;
    QLabel *mWordCntLabel;
    QLineEdit *mFilePathEdit;
    QLabel *mSaveStatusLabel;
};

#endif // MAINWINDOW_H

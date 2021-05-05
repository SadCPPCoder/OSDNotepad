/*******************************************
 * File Name: mainwindow.h
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Declare the main window for
 * the OSD Notepad application.
 *******************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFontComboBox>
#include <QFile>
#include <QLabel>
#include <QTabWidget>
#include <QListWidget>
#include "about.h"
#include "bzcombobox.h"
#include "osdcolorcombobox.h"
#include "osdfontcombobox.h"
#include "osdspinbox.h"
#include "osdshortcutdialog.h"
#include "osdtextedit.h"
#include "searchbar.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString file, QWidget *parent = 0);
    ~MainWindow();

protected:
    void mouseReleaseEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void closeEvent(QCloseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void dragEnterEvent(QDragEnterEvent *event);
    void dropEvent(QDropEvent *event);

private:
    void setupExtraUi();
    bool addNewEditPage(const QString &fileName);
    void setFilePath(const QString& path);
    void openLastFile();
    bool fileFormatMatch(const QString& fileName);
    void updateFileRelatedUI();
    void updateCharRelatedUI();
    QString closeEditPage(OSDTextEdit *edit, const int index);
    void copyCharFormat();
    void pasteCharFormat();
    QString saveFile(OSDTextEdit *edit, bool saveAs);
    QString openFile();

private slots:
    void fontFamilyChanged(const QString& family);
    void fontSizeChanged(int size);
    void foregroundColorChanged(const QColor& color);
    void backgroundColorChanged(const QColor& color);
    void textEditCursorPositionChanged();
    void textEditTextChanged();
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
    void on_actionAbout_triggered();
    void on_currentChanged(int index);
    void on_tabCloseRequested(int index);
    void on_updateListContent();
    void on_recentFileComboboxListItemDoubleClicked(QListWidgetItem *item);
    bool eventFilter(QObject *watched, QEvent *event);
    void on_actionSaveAs_triggered();
    void on_actionShortcut_triggered();
    void on_actionTable_triggered();
    void on_insertListBtnClicked();

private:
    Ui::MainWindow *ui;
    OSDFontComboBox *mFontCobox;
    OSDSpinBox *mFontSizeSpinbox;
    OSDColorComboBox *mForeColorCobox;
    OSDColorComboBox *mBackColorCobox;
    OSDTextEdit *mTextEdit;
    bool mFileModifiedFlag;
    int mImgMinSize;
    QLabel *mPositionLabel;
    QLabel *mWordCntLabel;
    QLineEdit *mFilePathEdit;
    QLabel *mSaveStatusLabel;
    About *mAbout;
    QTabWidget *mTabWgt;
    BZCombobox *mOpenRecentCombobox;
    QListWidget *mRecentListWgt;
    QPoint mMousePoint;
    bool mMousePressed;
    SearchBar mSearchBar;
    QFont mCopiedFont;
    QColor mCopiedTextClr;
    QColor mCopiedBackClr;
    OSDShortcutDialog *mShortcutDlg;
    QPushButton *mInsertListBtn;
};

#endif // MAINWINDOW_H

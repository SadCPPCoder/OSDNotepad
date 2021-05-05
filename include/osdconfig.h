/*******************************************
 * File Name: osdconfig.h
 * Date: 2021-01-30
 * Author: Bob.Zhang
 *
 * Description: Declare the single instance
 * to read/write the configuation file of the
 * application.
 *******************************************/

#ifndef OSDCONFIG_H
#define OSDCONFIG_H

#include <QKeySequence>
#include <QList>
#include <QLockFile>
#include <QMap>
#include <QPair>
#include <QString>

#define CONFIG_FILE_NAME "OSDNotepad.conf"
#define UNCLOSED_FILES "UnClosedFiles"
#define RECENT_FILES "RecentFiles"
#define CFG_SEPARATOR '='
#define CFG_SUB_SEPARATOR ','
#define CFG_CONFIG_LINE_SEPARATOR '#'
#define MAX_UNCLOSED_FILES 10
#define MAX_Recent_FILES 20
#define LAST_OPENED_DIR "LastOpenedDir"
#define LAST_OPENED_IMG_DIR "LastOpenedImgDir"
#define SK_FontSizeIncrease "FontSizeIncrease"
#define SK_FontSizeDecrease "FontSizeDecrease"
#define SK_SearchBarShow "SearchBarShow"
#define SK_SearchBarHide "SearchBarHide"
#define SK_FormatCopy "FormatCopy"
#define SK_FormatPaste "FormatPaste"
#define SK_TextRShift "TextRShift"
#define SK_TextLShift "TextLShift"
#define SK_SmartIndent "SmartIndent"
#define SK_HAlignment "HAlignment"
#define SK_VAlignment "VAlignment"
#define SK_TableRShift "TableRShift"
#define SK_TableLShift "TableLShift"

class OSDConfig : public QObject
{
    Q_OBJECT

    typedef
        QMap<QString, QPair<QKeySequence, QString>>
        OSDKeySeqMap;
public:
    static OSDConfig& getInstance();
    const QList<QString>& getUnclosedFileList() const;
    void cleanUnclosedFile();
    void appendUnclosedFile(const QString& file);
    const QList<QString>& getRecentFileList() const;
    void appendRecentFile(const QString& file);
    void removeRecentFile(const QString& file);
    void updateConfigFile();
    const QString& getLastOpenedDir() const;
    void updateLastOpenedDir(const QString& dir);
    const QString& getLastOpenedImgDir() const;
    void updateLastOpenedImgDir(const QString& dir);
    const QKeySequence& getKeySequence(const QString &KeyItem);
    const OSDKeySeqMap& getKeySequenceMapRef() const;

private:
    static OSDConfig *msConfig;
    static const QList<QString> msConfigStrList;

    QList<QString> mUnClosedFileList;
    QList<QString> mRecentFileList;
    QString mConfigFileName;
    QString mLastOpenedDir;
    QString mLastOpenedImgDir;
    OSDKeySeqMap mCustomKeys;
    QLockFile mConfigLockFile;

    OSDConfig(const QString &fileName, QObject *parent = NULL);
    void updateConfigItem(const QString& key, const QString& value);
};

#endif // OSDCONFIG_H

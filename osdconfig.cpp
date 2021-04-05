/*******************************************
 * File Name: osdconfig.cpp
 * Date: 2021-01-30
 * Author: Bob.Zhang
 *
 * Description: Implement the single instance
 * to read/write the configuation file of the
 * application.
 *******************************************/

#include <QCoreApplication>
#include <QFile>
#include <QMap>
#include "osdconfig.h"

OSDConfig *OSDConfig::msConfig = nullptr;
const QList<QString> OSDConfig::msConfigStrList =
{
    UNCLOSED_FILES,
    RECENT_FILES,
    LAST_OPENED_DIR,
};

OSDConfig::OSDConfig(const QString &fileName, QObject *parent)
    : QObject(parent)
    , mConfigFileName(fileName)
{
    // Set Shortcut Keys Default Value
    mCustomKeys.insert(SK_FontSizeIncrease,
                    qMakePair(QKeySequence(Qt::CTRL + Qt::Key_Equal),
                              tr("Font Size Increase")));
    mCustomKeys.insert(SK_FontSizeDecrease,
                    qMakePair(QKeySequence(Qt::CTRL + Qt::Key_Minus),
                              tr("Font Size Decrease")));
    mCustomKeys.insert(SK_SearchBarShow,
                    qMakePair(QKeySequence(Qt::CTRL + Qt::Key_F),
                              tr("Search Bar Show")));
    mCustomKeys.insert(SK_SearchBarHide,
                    qMakePair(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_F),
                              tr("Search Bar Hide")));
    mCustomKeys.insert(SK_FormatCopy,
                    qMakePair(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_C),
                              tr("Font Format Copy")));
    mCustomKeys.insert(SK_FormatPaste,
                    qMakePair(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_V),
                              tr("Font Format Paste")));
    mCustomKeys.insert(SK_TextRShift,
                    qMakePair(QKeySequence(Qt::Key_Tab),
                              tr("Selected Lines Right Shift")));
    mCustomKeys.insert(SK_TextLShift,
                    qMakePair(QKeySequence(Qt::SHIFT + Qt::Key_Backtab),
                              tr("Current Lines Left Shift")));
    mCustomKeys.insert(SK_SmartIndent,
                    qMakePair(QKeySequence(Qt::Key_Return),
                              tr("Smart Indent")));

    QMap<QString, QString> cfgMap;
    QFile cfgFile(mConfigFileName);
    if(!cfgFile.exists())
    {
        return;
    }

    cfgFile.open(QIODevice::ReadOnly);
    QString cfgStrings(cfgFile.readAll());
    cfgFile.close();

    auto cfgList = cfgStrings.split(CFG_CONFIG_LINE_SEPARATOR);

    // get all configurations
    QString cfg;
    foreach (cfg, cfgList)
    {
        int index = cfg.indexOf(CFG_SEPARATOR);
        if(index > 0)
        {
            cfgMap.insert(cfg.left(index), cfg.right(cfg.length()-index-1));
        }
    }

    // get the configuration we wanted
    foreach(cfg, msConfigStrList)
    {
        if(!cfgMap[cfg].isEmpty())
        {
            updateConfigItem(cfg, cfgMap[cfg]);
        }
    }
}

void OSDConfig::updateConfigItem(const QString &key, const QString &value)
{
    if(UNCLOSED_FILES == key)
    {
        mUnClosedFileList.append(value.split(CFG_SUB_SEPARATOR));
    }
    else if(RECENT_FILES == key)
    {
        mRecentFileList.append(value.split(CFG_SUB_SEPARATOR));
    }
    else if(LAST_OPENED_DIR == key)
    {
        mLastOpenedDir = value;
    }
    else if(LAST_OPENED_IMG_DIR == key)
    {
        mLastOpenedImgDir = value;
    }
}

OSDConfig &OSDConfig::getInstance()
{
    if(nullptr == msConfig)
    {
        QString confFile = QCoreApplication::applicationFilePath()
                + "/" + CONFIG_FILE_NAME;
        msConfig = new OSDConfig(CONFIG_FILE_NAME,
                                 QCoreApplication::instance());
    }

    return *msConfig;
}

const QList<QString>& OSDConfig::getUnclosedFileList() const
{
    return mUnClosedFileList;
}

void OSDConfig::cleanUnclosedFile()
{
    mUnClosedFileList.clear();
}

void OSDConfig::appendUnclosedFile(const QString &file)
{
    if(mUnClosedFileList.length() < MAX_UNCLOSED_FILES &&
       !mUnClosedFileList.contains(file) &&
       !file.isEmpty())
    {
        mUnClosedFileList.append(file);
    }
}

const QList<QString>& OSDConfig::getRecentFileList() const
{
    return mRecentFileList;
}

void OSDConfig::appendRecentFile(const QString &file)
{
    if(!file.isEmpty())
    {
        auto index = mRecentFileList.indexOf(file);
        if(index >= 0)
        {
            mRecentFileList.removeAt(index);
        }
        else if(mRecentFileList.size() >= MAX_Recent_FILES)
        {
            mRecentFileList.removeLast();
        }

        mRecentFileList.push_front(file);
    }
}

void OSDConfig::removeRecentFile(const QString &file)
{
    mRecentFileList.removeAll(file);
}

void OSDConfig::updateConfigFile()
{
    QString cfgContent;
    QString str;
    int count = 0;

    cfgContent += UNCLOSED_FILES;
    cfgContent += CFG_SEPARATOR;
    foreach (str, mUnClosedFileList)
    {
        if(count++)
        {
            cfgContent += CFG_SUB_SEPARATOR;
        }
        cfgContent += str;
    }
    cfgContent += CFG_CONFIG_LINE_SEPARATOR;

    cfgContent += RECENT_FILES;
    cfgContent += CFG_SEPARATOR;
    count = 0;
    foreach (str, mRecentFileList)
    {
        if(count++)
        {
            cfgContent += CFG_SUB_SEPARATOR;
        }
        cfgContent += str;
    }
    cfgContent += CFG_CONFIG_LINE_SEPARATOR;

    cfgContent += LAST_OPENED_DIR;
    cfgContent += CFG_SEPARATOR;
    cfgContent += mLastOpenedDir;    
    cfgContent += CFG_CONFIG_LINE_SEPARATOR;

    cfgContent += LAST_OPENED_IMG_DIR;
    cfgContent += CFG_SEPARATOR;
    cfgContent += mLastOpenedImgDir;

    QFile cfgFile(mConfigFileName);
    cfgFile.open(QIODevice::WriteOnly);
    cfgFile.write(cfgContent.toUtf8());
    cfgFile.close();
}

const QString &OSDConfig::getLastOpenedDir() const
{
    return mLastOpenedDir;
}

void OSDConfig::updateLastOpenedDir(const QString &dir)
{
    if(!dir.isEmpty())
    {
        mLastOpenedDir = dir;
    }
}

const QString &OSDConfig::getLastOpenedImgDir() const
{
    return mLastOpenedImgDir;
}

void OSDConfig::updateLastOpenedImgDir(const QString &dir)
{
    if(!dir.isEmpty())
    {
        mLastOpenedImgDir = dir;
    }
}

const QKeySequence& OSDConfig::getKeySequence(const QString &KeyItem)
{
    return mCustomKeys[KeyItem].first;
}

const OSDConfig::OSDKeySeqMap &OSDConfig::getKeySequenceMapRef() const
{
    return mCustomKeys;
}

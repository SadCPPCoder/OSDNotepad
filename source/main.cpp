/*******************************************
 * File Name: main.cpp
 * Date: 2020-11-20
 * Author: Bob.Zhang
 *
 * Description: Main function for the application.
 *******************************************/

#include <QApplication>
#include <QTextCodec>
#include <QTranslator>
#include "mainwindow.h"
#include "osdconfig.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);

    QTranslator translator;
    QLocale locale;
    QString lang = locale.language() == QLocale::Chinese ?
                ":/language/lang_Chinses.qm" : ":/language/lang_English.qm";
    translator.load(lang);
    QTranslator qtLang;
    QTranslator qtbaseLang;
    if(locale.language() == QLocale::Chinese)
    {
        qtLang.load(":/language/qt_zh_CN.qm");
        qtbaseLang.load(":/language/qtbase_zh_CN.qm");
        a.installTranslator(&qtLang);
        a.installTranslator(&qtbaseLang);
    }
    a.installTranslator(&translator);
    a.setWindowIcon(QIcon(":/res/logo/logo.png"));

    QString file;
    if(argc > 1)
    {
        file = argv[1];
    }

    MainWindow w(file);
    w.show();

    return a.exec();
}

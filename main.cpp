#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTextCodec *codec = QTextCodec::codecForName("System");
    QTextCodec::setCodecForLocale(codec);

    QTranslator translator;
    QLocale locale;
    QString lang = locale.language() == QLocale::Chinese ?
                ":/lang_Chinses.qm" : ":/lang_English.qm";
    translator.load(lang);
    a.installTranslator(&translator);

    MainWindow w;
    w.show();

    return a.exec();
}

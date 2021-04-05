#-------------------------------------------------
#
# Project created by QtCreator 2020-10-23T20:26:01
#
#-------------------------------------------------

QT       += core gui \
            printsupport
RC_ICONS = res/logo/logo.ico

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = OSDNotepad
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \
    osdtextedit.cpp \
    osdfontcombobox.cpp \
    osdspinbox.cpp \
    osdcolorcombobox.cpp \
    about.cpp \
    searchbar.cpp \
    osdconfig.cpp \
    osdshortcutdialog.cpp

HEADERS += \
        mainwindow.h \
    osdtextedit.h \
    osdfontcombobox.h \
    osdspinbox.h \
    osdcolorcombobox.h \
    about.h \
    globalinfo.h \
    searchbar.h \
    osdconfig.h \
    osdshortcutdialog.h

FORMS += \
        mainwindow.ui \
    about.ui \
    searchbar.ui \
    osdshortcutdialog.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS = lang_English.ts \
               lang_Chinses.ts

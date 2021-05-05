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
    source/about.cpp \
    source/main.cpp \
    source/mainwindow.cpp \
    source/osdcolorcombobox.cpp \
    source/osdconfig.cpp \
    source/osdfontcombobox.cpp \
    source/osdshortcutdialog.cpp \
    source/osdspinbox.cpp \
    source/osdtextedit.cpp \
    source/searchbar.cpp \
    source/bzcombobox.cpp

INCLUDEPATH += \
    include

HEADERS += \
    include/about.h \
    include/globalinfo.h \
    include/mainwindow.h \
    include/osdcolorcombobox.h \
    include/osdconfig.h \
    include/osdfontcombobox.h \
    include/osdshortcutdialog.h \
    include/osdspinbox.h \
    include/osdtextedit.h \
    include/searchbar.h \
    include/bzcombobox.h

FORMS += \
    design/about.ui \
    design/mainwindow.ui \
    design/osdshortcutdialog.ui \
    design/searchbar.ui \
    design/bzcombobox.ui

RESOURCES += \
    resources.qrc

TRANSLATIONS = \
    language/lang_Chinses.ts \
    language/lang_English.ts

DISTFILES += \
    res/color/black.png \
    res/color/blue.png \
    res/color/brown.png \
    res/color/gray.png \
    res/color/green.png \
    res/color/orange.png \
    res/color/perple.png \
    res/color/pink.png \
    res/color/red.png \
    res/color/white.png \
    res/color/yellow.png \
    res/icon/about.png \
    res/icon/alipay.png \
    res/icon/alway_on_top.png \
    res/icon/background_color.png \
    res/icon/bold.png \
    res/icon/bottom.png \
    res/icon/case.png \
    res/icon/foreground_color.png \
    res/icon/format-strike.png \
    res/icon/format-underline.png \
    res/icon/img_origin.png \
    res/icon/img_small.png \
    res/icon/insert_img.png \
    res/icon/italic.png \
    res/icon/list.png \
    res/icon/new_file.png \
    res/icon/opacity_decrease.png \
    res/icon/opacity_increase.png \
    res/icon/open_file.png \
    res/icon/open_recent_file.png \
    res/icon/papal.png \
    res/icon/reg.png \
    res/icon/reset.png \
    res/icon/save_file.png \
    res/icon/save_file_as.png \
    res/icon/search.png \
    res/icon/shortcut.png \
    res/icon/table.png \
    res/icon/view_off.png \
    res/icon/view_on.png \
    res/icon/wechat.png \
    res/logo/logo.png \
    res/alipay.png \
    res/wechat.png \
    res/logo/logo.ico \
    res/logo/logo.xcf \
    language/lang_Chinses.qm \
    language/lang_English.qm \
    language/qt_zh_CN.qm \
    language/qtbase_zh_CN.qm

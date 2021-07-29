#-------------------------------------------------
#
# Project created by QtCreator 2019-06-24T21:31:38
#
#-------------------------------------------------

QT       += core
QT       += gui
QT       += widgets
QT       += xml
QT       += network

greaterThan(QT_MAJOR_VERSION, 6) {
    QT       += core5compat
}

!versionAtLeast(QT_VERSION, 5.15):error("Use at least Qt version 5.15")

VERSION = 2.4.7.6

macx: ICON = diskbutler.icns
macx: DEFINES += MAC
TARGET = Diskbutler
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += APP_VERSION=\\\"$$VERSION\\\"
win32: DEFINES += WIN32 UNICODE _UNICODE
unix:!macx {
DEFINES += LINUX
}

DEFINES += _CRT_SECURE_NO_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
include($$PWD/ppbrowser/src/qtpropertybrowser.pri)

CONFIG += c++11

HEADERS_CONTROLS = \
    controls/devicecomboex.h \
    controls/qtwaitingspinner.h
SOURCES_CONTROLS = \
    controls/devicecomboex.cpp \
    controls/qtwaitingspinner.cpp
HEADERS_FOXSDK = \
    foxsdk/bass.h \
    foxsdk/tchar.h \
    foxsdk/FoxSDKBurningLib.h \
    foxsdk/FoxSDKExport.h \
    foxsdk/FoxSDKDefinitions.h \
    foxsdk/FoxSDKUnicode.h
HEADERS_NODES = \
    nodes/qdataitem.h \
    nodes/qdiskitem.h \
    nodes/qaudiotrackitem.h \
    nodes/qmediumitem.h \
    nodes/qsessionitem.h \
    nodes/qtrackitem.h \
    nodes/qfilesysitem.h \
    nodes/qfilesys_iso9660item.h \
    nodes/qfilesys_udfitem.h \
    nodes/qfolderitem.h \
    nodes/qfileitem.h
SOURCES_NODES = \
    nodes/qdataitem.cpp \
    nodes/qdiskitem.cpp \
    nodes/qaudiotrackitem.cpp \
    nodes/qmediumitem.cpp \
    nodes/qsessionitem.cpp \
    nodes/qtrackitem.cpp \
    nodes/qfilesysitem.cpp \
    nodes/qfilesys_iso9660item.cpp \
    nodes/qfilesys_udfitem.cpp \
    nodes/qfolderitem.cpp \
    nodes/qfileitem.cpp
HEADERS_UTILS = \
    utils/utils_common.h \
    utils/messanger.h \
    utils/vrulemanager.h \
    utils/dynbasslib.h
SOURCES_UTIS = \
    utils/utils_common.cpp \
    utils/messanger.cpp \
    utils/vrulemanager.cpp \
    utils/dynbasslib.cpp
HEADERS_RIBBON = \
    ribbon/ribbon.h \
    ribbon/ribbonbuttongroup.h \
    ribbon/ribbonbuilder.h \
    ribbon/ribbontabcontent.h
SOURCES_RIBBON = \
    ribbon/ribbon.cpp \
    ribbon/ribbonbuttongroup.cpp \
    ribbon/ribbontabcontent.cpp
HEADERS_ZMODULES = \
    zmodules/zfileiconprovider.h \
    zmodules/zimageloader.h \
    zmodules/zplaylistloader.h \
    zmodules/zreplacefiledialog.h
SOURCES_ZMODULES = \
    zmodules/zfileiconprovider.cpp \
    zmodules/zimageloader.cpp \
    zmodules/zplaylistloader.cpp \
    zmodules/zreplacefiledialog.cpp
HEADERS_DIALOGS = \
    dialogs/burndialog.h \
    dialogs/vcddialog.h \
    dialogs/vdiskpropertydialog.h \
    dialogs/vpropertydialog.h \
    dialogs/zpropertydialogfoxfile.h
SOURCES_DIALOGS = \
    dialogs/burndialog.cpp \
    dialogs/vcddialog.cpp \
    dialogs/vdiskpropertydialog.cpp \
    dialogs/vpropertydialog.cpp \
    dialogs/zpropertydialogfoxfile.cpp
HEADERS_PROJECT = \
    project/zproject_bluray.h \
    project/zproject_explore.h \
    project/zproject_videodvd.h
SOURCES_PROJECT = \
    project/zproject_bluray.cpp \
    project/zproject_explore.cpp \
    project/zproject_videodvd.cpp
HEADERS_STRATEGY = \
    strategy/zimport_strategy.h \
    strategy/zimport_strategy_fixedfolder.h \
    strategy/zimport_strategy_nosubfolder.h \
    strategy/zimport_strategy_suffixallowed.h
SOURCES_STRATEGY = \
    strategy/zimport_strategy_fixedfolder.cpp \
    strategy/zimport_strategy_nosubfolder.cpp \
    strategy/zimport_strategy_suffixallowed.cpp
HEADERS_MODEL = \
    model/zinfotablemodel.h \
    model/zallocation_table_model.h
SOURCES_MODEL = \
    model/zinfotablemodel.cpp \
    model/zallocation_table_model.cpp
HEADERS_HEX = \
    hex/qhexview.h \
    hex/qdummytexttree.h \
    hex/qscanboard.h
SOURCES_HEX = \
    hex/qhexview.cpp \
    hex/qscanboard.cpp \
    hex/qdummytexttree.cpp

SOURCES += $${SOURCES_CONTROLS} \
        $${SOURCES_NODES} \
        $${SOURCES_UTIS} \
        $${SOURCES_RIBBON} \
        $${SOURCES_ZMODULES} \
        $${SOURCES_DIALOGS} \
        $${SOURCES_PROJECT} \
        $${SOURCES_STRATEGY} \
        $${SOURCES_MODEL} \
        $${SOURCES_HEX} \
        commontreewidget.cpp \
        audiotaginfo.cpp \
        configdialog.cpp \
        main.cpp \
        mainwindow.cpp \
        mdichild_base.cpp \
        mdichild_deviceinfo.cpp \
        mdichild_dialog.cpp \
        mdichild_diskinfo.cpp \
        mdichild_hex.cpp \
        mdichild_scan.cpp \
        settingspages.cpp \        
        xbelgenerator.cpp \
        xbelhandler.cpp


HEADERS += $${HEADERS_CONTROLS} \
        $${HEADERS_FOXSDK} \
        $${HEADERS_NODES} \
        $${HEADERS_UTILS} \
        $${HEADERS_RIBBON} \
        $${HEADERS_ZMODULES} \
        $${HEADERS_DIALOGS} \
        $${HEADERS_PROJECT} \
        $${HEADERS_STRATEGY} \
        $${HEADERS_MODEL} \
        $${HEADERS_HEX} \
        commontreewidget.h \
        audiotaginfo.h \
        configdialog.h \
        foxsdk/bassmix.h \
        foxsdk/tags.h \
        mainwindow.h \
        mdichild_base.h \
        mdichild_deviceinfo.h \
        mdichild_dialog.h \
        mdichild_diskinfo.h \
        mdichild_hex.h \
        mdichild_scan.h \
        settingspages.h \
        xbelgenerator.h \
        xbelhandler.h

TRANSLATIONS = diskbutler_de.ts
CODECFORSRC = UTF-8

OTHER_FILES += \
  diskbutler_de.ts

FORMS += \
        dialogs/burndialog.ui \
        dialogs/vcddialog.ui \
        dialogs/vdiskpropertydialog.ui \
        dialogs/vpropertydialog.ui \
        dialogs/zpropertydialogfoxfile.ui \
        mainwindow.ui \
        ribbon/ribbonbuttongroup.ui \
        ribbon/ribbontabcontent.ui \
        zmodules/zreplacefiledialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    diskbutler.qrc

INCLUDEPATH += $$PWD $$PWD/project $$PWD/nodes $$PWD/strategy $$PWD/foxsdk $$PWD/utils $$PWD/dialogs $$PWD/model
INCLUDEPATH += $$PWD $$PWD/zmodules $$PWD/ribbon $$PWD/controls $$PWD/hex
DEPENDPATH += $$PWD/foxsdk

win32: LIBS += -L$$PWD/foxsdk/ -lFoxSDKCore
win32: PRE_TARGETDEPS += $$PWD/foxsdk/FoxSDKCore.lib

unix:!macx {
LIBS += -L$$PWD/foxsdk/ -lFoxSDKCore
PRE_TARGETDEPS += $$PWD/foxsdk/libFoxSDKCore.so
}

macx:{
LIBS += -L$$PWD/foxsdk/ -lFoxSDKCore
MediaFiles.files += FoxSDK/libFoxSDKCore.dylib
MediaFiles.files += diskbutler_de.qm
MediaFiles.path = Contents/MacOS
QMAKE_BUNDLE_DATA += MediaFiles
}

DISTFILES += \
    diskbutler_de.ts

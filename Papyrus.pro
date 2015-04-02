contains(BUILD_MODE,ubuntutouch) {
    DESTDIR = land.aseman.papyrus
}

folder_01.source = files/fonts
folder_01.target = $$DESTDIR/.
folder_02.source = files/translations
folder_02.target = $$DESTDIR/files
folder_03.source = files/KFaenza
folder_03.target = $$DESTDIR/files/icons/
DEPLOYMENTFOLDERS += folder_01 folder_02 folder_03

QT += sql positioning qml quick xml

contains(BUILD_MODE,free) {
    DEFINES += FREE_BUILD
}
contains(BUILD_MODE,trial) {
    DEFINES += FREE_BUILD
    DEFINES += TRIAL_BUILD
}
contains(BUILD_MODE,superdebug) {
    DEFINES += SUPER_DEBUG
}

ios {
    QT += multimedia
    QTPLUGIN += qsqlite
    QMAKE_INFO_PLIST = iOS/info.plist

    folder_04.source = iOS/splash/Default-568h@2x.png
    folder_04.target = .
    folder_05.source = iOS/icons
    folder_05.target = files
    DEPLOYMENTFOLDERS += folder_04 folder_05
} else:android {
    QT += multimedia
    contains(DEFINES,FREE_BUILD) {
        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android/free
    } else {
        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android/pro
    }
} else:contains(BUILD_MODE,ubuntutouch) {
    folder_04.source = ubuntutouch/papyrus.desktop
    folder_04.target = $$DESTDIR
    folder_05.source = ubuntutouch/papyrus.json
    folder_05.target = $$DESTDIR
    folder_06.source = ubuntutouch/manifest.json
    folder_06.target = $$DESTDIR
    folder_07.source = ubuntutouch/icons
    folder_07.target = $$DESTDIR
    DEPLOYMENTFOLDERS += folder_04 folder_05 folder_06 folder_07
} else {
    QT += widgets

    HEADERS += \
        iconprovider.h \
        desktop/papyrusdesktop.h \
        desktop/categoriesmodel.h \
        desktop/panelbox.h \
        desktop/datesmodel.h \
        desktop/papersview.h \
        desktop/paperviewdelegate.h \
        desktop/papersmodel.h \
        desktop/editorview.h \
        desktop/editorviewmanager.h \
        desktop/searchpanel.h \
        desktop/groupbutton.h \
        desktop/addgroupdialog.h \
        desktop/paperconfigure.h \
        desktop/mapwidget.h \
        desktop/datewidget.h \
        desktop/configurepage.h \
        desktop/aboutdialog.h \
        desktop/simage.h \
        desktop/paperfilesview.h \
        desktop/paperfilesmodel.h \
        desktop/todopapers.h \
        desktop/scolor.h \
        desktop/papertextarea.h \
        desktop/todopapersitem.h \
        desktop/uiselectordialog.h

    SOURCES += \
        iconprovider.cpp \
        desktop/papyrusdesktop.cpp \
        desktop/categoriesmodel.cpp \
        desktop/panelbox.cpp \
        desktop/datesmodel.cpp \
        desktop/papersview.cpp \
        desktop/paperviewdelegate.cpp \
        desktop/papersmodel.cpp \
        desktop/editorview.cpp \
        desktop/editorviewmanager.cpp \
        desktop/searchpanel.cpp \
        desktop/groupbutton.cpp \
        desktop/addgroupdialog.cpp \
        desktop/paperconfigure.cpp \
        desktop/mapwidget.cpp \
        desktop/datewidget.cpp \
        desktop/configurepage.cpp \
        desktop/aboutdialog.cpp \
        desktop/simage.cpp \
        desktop/paperfilesview.cpp \
        desktop/paperfilesmodel.cpp \
        desktop/todopapers.cpp \
        desktop/scolor.cpp \
        desktop/papertextarea.cpp \
        desktop/todopapersitem.cpp \
        desktop/uiselectordialog.cpp

    FORMS += \
        desktop/paperconfigure.ui \
        desktop/configurepage.ui \
        desktop/aboutdialog.ui\
        desktop/uiselectordialog.ui

    win32 {
        QT += winextras
        RC_FILE = files/papyrus.rc
    }
}

SOURCES += main.cpp \
    papyrus.cpp \
    papermanager.cpp \
    repository.cpp \
    resourcemanager.cpp \
    papyrussync.cpp \
    backuper.cpp \
    database.cpp \
    papyrusdropbox.cpp \
    smartiodbox.cpp \
    smartiodboxsingle.cpp \
    searchhighlighter.cpp \
    databasedatatypes.cpp \
    translationmodel.cpp \
    weathermodel.cpp \
    papyruslabelsmodel.cpp \
    papyrushistorymodel.cpp \
    papyrussidebarmodel.cpp

HEADERS += \
    papyrus.h \
    papermanager.h \
    repository.h \
    resourcemanager.h \
    papyrussync.h \
    backuper.h \
    papyrusmacros.h \
    database.h \
    databasedatatypes.h \
    papyrusdropbox.h \
    asemandeveloper_macros.h \
    smartiodbox.h \
    smartiodboxsingle.h \
    searchhighlighter.h \
    translationmodel.h \
    weathermodel.h \
    papyruslabelsmodel.h \
    papyrushistorymodel.h \
    papyrussidebarmodel.h

TRANSLATIONS += \
    files/translations/lang-en.qm \
    files/translations/lang-fa.qm

FONTS += \
    files/fonts/DroidKaqazSans.ttf \
    files/fonts/DroidSansMono.ttf

OTHER_FILES += \
    android/pro/AndroidManifest.xml \
    android/free/AndroidManifest.xml \
    android-build/src/org/aseman/android/AsemanActivity.java \
    android-build/src/org/aseman/android/AsemanApplication.java \
    android-build/src/org/aseman/android/AsemanJavaLayer.java \
    iOS/Info.plist \
    files/database/database.sqlite

RESOURCES += \
    resource.qrc

include(qmake/qtcAddDeployment.pri)
include(qtdropbox/qtdropbox.pri)
include(asemantools/asemantools.pri)
include(asemantools/qmake/clickPkg.pri)
include(qmapcontrol/qmapcontrol.pri)
include(simpleqtcryptor/simpleqtcryptor.pri)
qtcAddDeployment()

isEmpty(PREFIX) {
    PREFIX = /usr
}

contains(BUILD_MODE,opt) {
    BIN_PATH = $$PREFIX/
    SHARES_PATH = $$PREFIX/
    APPDESK_PATH = /usr/
    APPDESK_SRC = files/shortcuts/opt/
} else {
    BIN_PATH = $$PREFIX/bin
    SHARES_PATH = $$PREFIX/share/papyrus/
    APPDESK_PATH = $$PREFIX/
    APPDESK_SRC = files/shortcuts/normal/
}

android {
} else:contains(BUILD_MODE,ubuntutouch) {
    CONFIG(debug, debug|release) {
    } else {
        buildClickPkg()
    }
} else:linux {
    target = $$TARGET
    target.path = $$BIN_PATH
    translations.files = $$TRANSLATIONS
    translations.path = $$SHARES_PATH/files/translations
    fonts.files = $$FONTS
    fonts.path = $$SHARES_PATH/fonts/
    icons.files = files/icons/papyrus.png
    icons.path = $$SHARES_PATH/icons/
    faenza.files = files/KFaenza
    faenza.path = $$SHARES_PATH/files/icons/
    desktopFile.files = $$APPDESK_SRC/Papyrus.desktop
    desktopFile.path = $$APPDESK_PATH/share/applications
    desktopTouchFile.files = $$APPDESK_SRC/Papyrus-touch.desktop
    desktopTouchFile.path = $$APPDESK_PATH/share/applications

    INSTALLS = target fonts translations icons desktopFile desktopTouchFile faenza
}

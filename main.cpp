/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Papyrus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Papyrus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "papyrusmacros.h"
#include "papyrus.h"
#include "resourcemanager.h"
#include "asemantools/asemanapplication.h"
#include "asemantools/asemantools.h"

#ifdef DESKTOP_DEVICE
#include "desktop/uiselectordialog.h"
#endif

#include <QFile>
#include <QIcon>
#include <QSettings>
#include <QDebug>
#include <QDir>

#ifdef Q_OS_ANDROID
#define SIALAN_KAQAZ_HOME_PATH QDir::homePath().replace("land.aseman","org.sialan").replace("papyrus","kaqaz").replace("aseman","sialan")
#else
#ifdef Q_OS_IOS
#define SIALAN_KAQAZ_HOME_PATH QDir::homePath().replace("land.aseman","org.sialan").replace("papyrus","kaqaz").replace("aseman","sialan")
#else
#ifdef Q_OS_WIN
#define SIALAN_KAQAZ_HOME_PATH QDir::homePath() + "/AppData/Local/sialan/kaqaz"
#else
#ifdef Q_OS_UBUNTUTOUCH
#define SIALAN_KAQAZ_HOME_PATH QDir::homePath() + "/.config/org.sialan.kaqaz"
#else
#define SIALAN_KAQAZ_HOME_PATH QDir::homePath() + "/.config/sialan/kaqaz"
#endif
#endif
#endif
#endif

#ifdef Q_OS_ANDROID
#define SIALAN_KAQAZ_BACKUP_PATH "/sdcard/Sialan/Kaqaz/backups/"
#else
#ifdef Q_OS_IOS
#define SIALAN_KAQAZ_BACKUP_PATH QDir::homePath() + "/backups/"
#else
#define SIALAN_KAQAZ_BACKUP_PATH SIALAN_KAQAZ_HOME_PATH + "/backups/"
#endif
#endif

int main(int argc, char *argv[])
{
    AsemanApplication app(argc, argv);
    app.setApplicationName("Papyrus");
    app.setApplicationDisplayName("Papyrus");
    app.setOrganizationDomain("land.aseman");
    app.setOrganizationName("Aseman");
    app.setWindowIcon(QIcon(app.applicationDirPath()+"/qml/Papyrus/files/papyrus.png"));

    // Kaqaz compability
    const QString &homePath = AsemanApplication::homePath();
    if(!QFileInfo::exists(homePath) && QFileInfo::exists(SIALAN_KAQAZ_HOME_PATH))
        AsemanTools::copyDirectory(SIALAN_KAQAZ_HOME_PATH, homePath);
#ifdef Q_OS_ANDROID
    else
    if(!QFileInfo::exists(homePath) && QFileInfo::exists(QString(SIALAN_KAQAZ_HOME_PATH).replace("sialan.kaqaz","sialan.kaqaz.free")) )
        AsemanTools::copyDirectory(QString(SIALAN_KAQAZ_HOME_PATH).replace("sialan.kaqaz","sialan.kaqaz.free"), homePath);
#endif
    const QString &backupPath = AsemanApplication::backupsPath();
    if(!QFileInfo::exists(backupPath) && QFileInfo::exists(SIALAN_KAQAZ_BACKUP_PATH))
        AsemanTools::copyDirectory(SIALAN_KAQAZ_BACKUP_PATH, backupPath);

#ifdef DESKTOP_DEVICE
    if( app.isRunning() )
    {
        app.sendMessage("show");
        return 0;
    }
#endif

    Papyrus *papyrus = Papyrus::instance();

#ifdef DESKTOP_DEVICE
#ifdef Q_OS_MAC
    if( !papyrus->settings()->value("General/uiselected",false).toBool() )
    {
        UiSelectorDialog dialog;
        dialog.exec();

        if( !dialog.isAccepted() )
            return 0;
        if( dialog.isDesktopTouch() )
            papyrus->setDesktopTouchMode(true);
        else
            papyrus->setDesktopTouchMode(false);

        papyrus->settings()->setValue("General/uiselected",true);
    }
#else
    if( app.arguments().contains("--touch") )
        papyrus->setDesktopTouchMode(true);
#endif
#endif

    if( !papyrus->start() )
        return 0;

#ifdef DESKTOP_DEVICE
    QObject::connect( &app, SIGNAL(messageReceived(QString)), papyrus, SLOT(incomingAppMessage(QString)) );
#endif

    return app.exec();
}

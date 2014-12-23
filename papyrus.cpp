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

#define UNGROUPEDS_NODE "UNGROUPED"
#define TODAY_NODE      "TODAY"
#define GENERAL_NODE    "GENERAL"
#define ACTIVITY_NODE   "DEFAULT_ACTIVITY"

#include "papyrus.h"
#include "papermanager.h"
#include "repository.h"
#include "papyrussync.h"
#include "papyrusmacros.h"
#include "database.h"
#include "translationmodel.h"
#include "backuper.h"
#include "searchhighlighter.h"
#include "asemantools/asemantools.h"
#include "resourcemanager.h"
#include "weathermodel.h"
#include "simpleqtcryptor/simpleqtcryptor.h"
#include "asemantools/asemandevices.h"
#include "asemantools/asemanquickview.h"
#include "asemantools/asemancalendarconverter.h"

#include "qmapcontrol/osmmapadapter.h"
#include "qmapcontrol/maplayer.h"
#include "qmapcontrol/qmlmapcontrol.h"
#include "qmapcontrol/googlemapadapter.h"

#ifdef Q_OS_ANDROID
#include "asemantools/asemanjavalayer.h"
#endif

#ifdef DESKTOP_LINUX
#include "iconprovider.h"
#endif

#ifdef DESKTOP_DEVICE
#include "desktop/papyrusdesktop.h"
#include <QFileDialog>
#endif

#include <QGuiApplication>
#include <QClipboard>
#include <QTranslator>
#include <QQmlEngine>
#include <QQmlContext>
#include <QtQml>
#include <QDir>
#include <QDebug>
#include <QColor>
#include <QUuid>
#include <QHash>
#include <QScreen>
#include <QMimeDatabase>
#include <QDateTime>
#include <QFileInfo>
#include <QTimerEvent>
#include <QHash>
#include <QPalette>
#include <QSettings>
#include <QUrl>
#include <QDesktopServices>
#include <QProcess>
#include <QCryptographicHash>
#include <QFileSystemWatcher>
#include <QStandardPaths>

Database *papyrus_database = 0;
QSettings *papyrus_settings = 0;

using namespace qmapcontrol;

class PapyrusPrivate
{
public:
#ifdef DESKTOP_DEVICE
    PapyrusDesktop *viewer_classic;
#endif

    AsemanQuickView *viewer;

    QString homePath;
    QString translationsPath;
    QString confPath;
    QString profilePath;

    int lock_timer;
    bool close_blocker;
    bool keyboard;
    bool fullscreen;
    Papyrus::MapMode map_mode;

    bool demo_active_until_next;
    bool desktop_touch_mode;

    QTranslator *translator;
    PapyrusSync *sync;

    QHash<QString,QVariant> languages;
    QHash<QString,QLocale> locales;
    QString language;

    Repository *repository;

    QFileSystemWatcher *filesystem;
    QMimeDatabase mime_db;

    AsemanCalendarConverter *calendar;

#ifdef Q_OS_ANDROID
    AsemanJavaLayer *java_layer;
#endif
};

QPointer<Papyrus> papyrus_obj;
Papyrus::Papyrus(QObject *parent) :
    QObject(parent)
{
    papyrus_obj = this;

    p = new PapyrusPrivate;
    p->demo_active_until_next = false;
    p->desktop_touch_mode = false;
    p->lock_timer = 0;
    p->keyboard = false;
    p->fullscreen = false;
    p->viewer = 0;
#ifdef DESKTOP_DEVICE
    p->viewer_classic = 0;
#endif
    p->translator = new QTranslator(this);
#ifdef Q_OS_ANDROID
    p->java_layer = AsemanJavaLayer::instance();
#endif

    QDir().mkpath(CAMERA_PATH);

    p->filesystem = new QFileSystemWatcher(this);
    p->filesystem->addPath(CAMERA_PATH);

    const QStringList & camera_entryes = QDir(CAMERA_PATH).entryList(QDir::Dirs | QDir::NoDotAndDotDot);
    foreach( const QString & d, camera_entryes )
        p->filesystem->addPath( QString(CAMERA_PATH) + "/" + d );

    QDir().mkpath(TEMP_PATH);
    cleanCache();

#ifdef Q_OS_ANDROID
    p->close_blocker = true;
#else
    p->close_blocker = false;
#endif

    p->homePath = HOME_PATH;
#ifdef Q_OS_ANDROID
    p->translationsPath = "assets:/files/translations";
    p->confPath = p->homePath + "/config.ini";
#else
    p->translationsPath = resourcePath().mid(7) + "/files/translations/";
    p->confPath = p->homePath + "/config.ini";
#endif

    if( !papyrus_settings )
        papyrus_settings = new QSettings( p->confPath, QSettings::IniFormat );

    p->map_mode = static_cast<MapMode>(papyrus_settings->value("General/mapMode",GoogleMap).toInt());
    p->profilePath = papyrus_settings->value( "General/ProfilePath", p->homePath ).toString();
#ifdef Q_OS_MAC
    p->desktop_touch_mode = papyrus_settings->value("UserInterface/type", false).toBool();
#endif

    QDir().mkpath(p->homePath);
    QDir().mkpath(profilePath());
    QDir().mkpath(repositoryPath());

    p->repository = new Repository(this);

    if( !papyrus_database )
        papyrus_database = new Database();

    p->sync = new PapyrusSync(this);

    init_languages();

    qmlRegisterType<PaperManager>("Papyrus", 1,0, "PaperManager");
    qmlRegisterType<SearchHighlighter>("Papyrus", 1,0, "SearchHighlighter");
    qmlRegisterType<Enums>("Papyrus", 1,0, "Enums");
    qmlRegisterType<OSMMapAdapter>("Papyrus", 1,0, "OSMMapAdapter");
    qmlRegisterType<GoogleMapAdapter>("Papyrus", 1,0, "GoogleMapAdapter");
    qmlRegisterType<MapLayer>("Papyrus", 1,0, "MapLayer");
    qmlRegisterType<QmlMapControl>("Papyrus", 1,0, "MapControl");
    qmlRegisterType<TranslationModel>("Papyrus", 1,0, "TranslationModel");
    qmlRegisterType<Backuper>("Papyrus", 1,0, "Backuper");
    qmlRegisterType<WeatherData>("Papyrus", 1, 0, "WeatherData");
    qmlRegisterType<WeatherModel>("Papyrus", 1, 0, "WeatherModel");
    qmlRegisterUncreatableType<Papyrus>("Papyrus", 1,0, "Papyrus","");
    qmlRegisterUncreatableType<Database>("Papyrus", 1,0, "Database","");

    qRegisterMetaType<WeatherData>("WeatherData");
}

Papyrus *Papyrus::instance()
{
    if( !papyrus_obj )
        papyrus_obj = new Papyrus( QCoreApplication::instance() );

    return papyrus_obj;
}

QObject *Papyrus::view()
{
    return p->viewer;
}

PapyrusSync *Papyrus::papyrusSync() const
{
    return p->sync;
}

AsemanCalendarConverter *Papyrus::calendarConverter() const
{
    return p->calendar;
}

Repository *Papyrus::repository() const
{
    return p->repository;
}

void Papyrus::init_languages()
{
    QDir dir(p->translationsPath);
    QStringList languages = dir.entryList( QDir::Files );
    if( !languages.contains("lang-en.qm") )
        languages.prepend("lang-en.qm");

    for( int i=0 ; i<languages.size() ; i++ )
     {
         QString locale_str = languages[i];
             locale_str.truncate( locale_str.lastIndexOf('.') );
             locale_str.remove( 0, locale_str.indexOf('-') + 1 );

         QLocale locale(locale_str);

         QString  lang = QLocale::languageToString(locale.language());
         QVariant data = p->translationsPath + "/" + languages[i];

         p->languages.insert( lang, data );
         p->locales.insert( lang , locale );

         if( lang == papyrus_settings->value("General/Language","English").toString() )
             setCurrentLanguage( lang );
    }
}

bool Papyrus::start()
{
    if( p->viewer )
        return true;

#ifdef DESKTOP_DEVICE
    if( !p->desktop_touch_mode )
    {
        QIcon::setThemeSearchPaths( QStringList() << resourcePathAbs() + "/files/icons/" );
        QIcon::setThemeName("KFaenza");

        p->calendar = new AsemanCalendarConverter(this);

        p->viewer_classic = new PapyrusDesktop();
        p->viewer_classic->setDatabase(papyrus_database);
        p->viewer_classic->setRepository(p->repository);
        p->viewer_classic->setPapyrusSync(p->sync);
    }
    else
#endif
    {
        p->viewer = new AsemanQuickView(
#ifdef QT_DEBUG
                            AsemanQuickView::AllExceptLogger
#else
                            AsemanQuickView::AllComponents
#endif
                    );
        p->viewer->installEventFilter(this);
        p->viewer->engine()->rootContext()->setContextProperty( "papyrus", this );
        p->viewer->engine()->rootContext()->setContextProperty( "database", papyrus_database );
        p->viewer->engine()->rootContext()->setContextProperty( "filesystem", p->filesystem );
        p->viewer->engine()->rootContext()->setContextProperty( "repository", p->repository );
        p->viewer->engine()->rootContext()->setContextProperty( "sync", p->sync );
#ifdef DESKTOP_LINUX
        p->viewer->engine()->addImageProvider( "icon", new IconProvider() );
#endif

        connect(p->viewer->engine(), SIGNAL(quit()), SLOT(close()));

        connect( p->viewer->devices(), SIGNAL(incomingImage(QString)), SLOT(incomingImage(QString)) );
        connect( p->viewer->devices(), SIGNAL(incomingShare(QString,QString)), SLOT(incomingShare(QString,QString)) );
        connect( p->viewer->devices(), SIGNAL(selectImageResult(QString)), SLOT(selectImageResult(QString)) );
        connect( p->viewer->devices(), SIGNAL(activityPaused()), SLOT(activityPaused()) );
        connect( p->viewer->devices(), SIGNAL(activityResumed()), SLOT(activityResumed()) );

        p->calendar = p->viewer->calendar();
    }

    p->calendar->setCalendar( static_cast<AsemanCalendarConverterCore::CalendarTypes>(papyrus_settings->value("General/Calendar",AsemanCalendarConverterCore::Gregorian).toInt()) );

    connect( papyrus_database, SIGNAL(fileDeleted(QString)), p->repository, SLOT(deleteFile(QString)) );

    bool res = false;
#ifdef DESKTOP_DEVICE
    if( !p->desktop_touch_mode )
    {
        res = p->viewer_classic->start();
    }
    else
#endif
    {
        p->viewer->setSource(QStringLiteral("qrc:///qml/Papyrus/papyrus.qml"));
        p->viewer->show();
        res = true;
    }

#ifndef DESKTOP_DEVICE
    setFullscreen( papyrus_settings->value("UserInterface/fullscreen", false).toBool() );
#endif

    return res;
}

void Papyrus::incomingAppMessage(const QString &msg)
{
    if( msg == "show" )
    {
#ifdef DESKTOP_DEVICE
        if( !p->desktop_touch_mode )
        {
            p->viewer_classic->show();
            p->viewer_classic->activateWindow();
        }
        else
#endif
        {
            p->viewer->show();
            p->viewer->requestActivate();
        }
    }
}

void Papyrus::incomingShare(const QString &title, const QString &msg)
{
    if( !p->desktop_touch_mode )
        return;

    QVariant title_var = title;
    QVariant msg_var = msg;

    QMetaObject::invokeMethod( p->viewer->rootObject(), "incomingShare", Q_ARG(QVariant,title_var), Q_ARG(QVariant,msg_var) );
}

void Papyrus::incomingImage(const QString &path)
{
    if( !p->desktop_touch_mode )
        return;

    QVariant path_var = path;
    QMetaObject::invokeMethod( p->viewer->rootObject(), "incomingImage", Q_ARG(QVariant,path_var) );
}

void Papyrus::selectImageResult(const QString &path)
{
    qDebug() << "selectImageResult" << path;
}

void Papyrus::activityPaused()
{
    if( p->lock_timer )
        killTimer( p->lock_timer );

    p->lock_timer = startTimer( 30000 );
}

void Papyrus::activityResumed()
{
    if( !p->lock_timer )
        return;

    killTimer( p->lock_timer );
    p->lock_timer = 0;
}

bool Papyrus::demoHasTrial() const
{
#ifndef TRIAL_BUILD
    return true;
#endif

    if( p->demo_active_until_next )
        return true;
    if( DEMO_PAPERS_LIMIT >= database()->statePapersCount() )
        return true;

    return false;
}

void Papyrus::demoActiveTrial() const
{
    p->demo_active_until_next = true;
}

bool Papyrus::proBuild() const
{
#ifdef FREE_BUILD
    return false;
#else
    return true;
#endif
}

QString Papyrus::version()
{
    return PAPYRUS_VERSION
#ifndef FREE_BUILD
            " pro"
#else
#ifdef TRIAL_BUILD
            " trial"
#else
            " free"
#endif
#endif
            ;
}

void Papyrus::deleteFileIfPossible(const QString &id)
{
    if( papyrus_database->fileContaintOnDatabase(id) )
        return;

    const QString & path = p->repository->getPath(id);
    QFile::remove(path);
}

bool Papyrus::removeFile(const QString &path)
{
    return QFile::remove(path);
}

bool Papyrus::existsFile(const QString &path)
{
    return QFile::exists(path);
}

bool Papyrus::copyFile(const QString &src, const QString &dst)
{
    return QFile::copy(src,dst);
}

void Papyrus::setCalendar(int t)
{
    p->calendar->setCalendar( static_cast<AsemanCalendarConverterCore::CalendarTypes>(t) );
    papyrus_settings->setValue( "General/Calendar", t );
    emit calendarChanged();
}

QStringList Papyrus::calendarsID() const
{
    return p->calendar->calendarsID();
}

int Papyrus::calendar() const
{
    return papyrus_settings->value("General/Calendar",0).toInt();
}

void Papyrus::close()
{
    p->close_blocker = false;
    p->viewer->close();
}

void Papyrus::minimize()
{
    p->viewer->showMinimized();
}

Database *Papyrus::database()
{
    return papyrus_database;
}

QSettings *Papyrus::settings()
{
    return papyrus_settings;
}

void Papyrus::refreshSettings()
{
    if( papyrus_settings )
        delete papyrus_settings;

    papyrus_settings = new QSettings( p->confPath, QSettings::IniFormat );
}

void Papyrus::setClipboard(const QString &text)
{
    QGuiApplication::clipboard()->setText( text );
}

QString Papyrus::clipboard() const
{
    return QGuiApplication::clipboard()->text();
}

QStringList Papyrus::languages()
{
    QStringList res = p->languages.keys();
    res.sort();
    return res;
}

void Papyrus::setCurrentLanguage(const QString &lang)
{
    if( p->language == lang )
        return;

    QGuiApplication::removeTranslator(p->translator);
    p->translator->load(p->languages.value(lang).toString(),"languages");
    QGuiApplication::installTranslator(p->translator);
    p->language = lang;

    papyrus_settings->setValue("General/Language",lang);

    emit languageChanged();
    emit languageDirectionChanged();
}

QString Papyrus::currentLanguage() const
{
    return p->language;
}

void Papyrus::shareToFile(const QString &subject, const QString &message, const QString &path)
{
    if( QFile::exists(path) )
        QFile::remove(path);

    QFile file(path);
    if( !file.open(QFile::WriteOnly) )
        return;

    QString output = subject + "\n=-=-=-=-=-=-\n\n" + message;

    file.write(output.toUtf8());
    file.close();
}

QString Papyrus::cacheFile(const QString &address)
{
    QDir().mkpath(TEMP_PATH);
    QFileInfo file(address);
    QString dest = QString(TEMP_PATH) + "/papyrus_open_tmp." + file.suffix().toLower();
    QFile::remove(dest);
    QFile::copy(address,dest);
    return dest;
}

void Papyrus::cleanCache()
{
#ifdef Q_OS_ANDROID
    const QStringList & l = QDir(TEMP_PATH).entryList(QDir::Files);
    foreach( const QString & f, l )
        QFile::remove( QString(TEMP_PATH) + "/" + f );
#endif
}

QString Papyrus::getTempPath()
{
    QDir().mkpath(TEMP_PATH);
    QString filePath = QString(TEMP_PATH) + "/papyrus_temp_file_%1";

    int index = 0;
    while( QFile::exists(filePath.arg(index)) )
        index++;

    return filePath.arg(index);
}

QString Papyrus::getStaticTempPath()
{
    static QString path = getTempPath();
    return path;
}

void Papyrus::setProfilePath(QString path)
{
    if( path.isEmpty() )
        path = p->homePath;
    if( p->profilePath == path )
        return;

    const QString & old_db = profilePath() + "/database.sqlite";
    const QString & new_db = path + "/database.sqlite";
    const QString & old_rep  = repositoryPath();

    disconnectAllResources();

    p->profilePath = path;
    papyrus_settings->setValue( "General/ProfilePath", path );

    QDir().mkpath(profilePath());
    QDir().mkpath(repositoryPath());


    QFile::copy( old_db, new_db );

    const QStringList & rep_files = QDir(old_rep).entryList(QDir::Files);
    foreach( const QString & f, rep_files )
    {
        QFile::copy( old_rep + "/" + f, repositoryPath() + "/" + f );
        QFile::remove( old_rep + "/" + f );
    }

    papyrus_database->setPath( new_db );
    QFile::remove( old_db );

    reconnectAllResources();
}

QString Papyrus::profilePath() const
{
    return p->profilePath;
}

QString Papyrus::repositoryPath() const
{
    return profilePath() + "/repository";
}

QString Papyrus::sdcardPath() const
{
    return "/sdcard/Android/data/land.aseman.papyrus";
}

QString Papyrus::translatorFilePath() const
{
    return profilePath() + "/translate.ts";
}

QString Papyrus::resourcePathAbs()
{
#ifdef Q_OS_ANDROID
    return "assets:";
#else
    static QString *resourcePath = 0;
    if( !resourcePath )
    {
#ifdef Q_OS_MAC
        QFileInfo inf(QCoreApplication::applicationDirPath() + "/../Resources");
        resourcePath = new QString(inf.filePath());
#else
        QFileInfo inf(QCoreApplication::applicationDirPath()+"/../share/papyrus");
        if( inf.exists() )
            resourcePath = new QString(inf.filePath());
        else
            resourcePath = new QString(QCoreApplication::applicationDirPath());
#endif
    }
    return *resourcePath + "/";
#endif
}

QString Papyrus::resourcePath()
{
#ifdef Q_OS_ANDROID
    return resourcePathAbs();
#else
#ifdef Q_OS_WIN
    return "file:///" + resourcePathAbs();
#else
    return "file://" + resourcePathAbs();
#endif
#endif
}

QStringList Papyrus::findBackups()
{
    QString path = BACKUP_PATH;

    QStringList files = QDir(path).entryList( QStringList() << "*.kqz", QDir::Files, QDir::Size );
    for( int i=0; i<files.count(); i++ )
        files[i] = path + "/" + files[i];

    return files;
}

void Papyrus::disconnectAllResources()
{
    papyrus_database->disconnect();
}

void Papyrus::reconnectAllResources()
{
    papyrus_database->connect();
    refreshSettings();
#ifdef DESKTOP_DEVICE
    if( !p->desktop_touch_mode )
        p->viewer_classic->refresh();
    else
#endif
        QMetaObject::invokeMethod( p->viewer->rootObject(), "refresh" );
}

void Papyrus::setTutorialCompleted(bool stt)
{
    papyrus_settings->setValue("General/Tutorial",stt);
}

bool Papyrus::isTutorialCompleted() const
{
    return papyrus_settings->value("General/Tutorial",false).toBool();
}

void Papyrus::setGroupsCount(bool stt)
{
    papyrus_settings->setValue("General/GroupsCount",stt);
    emit groupsCountChanged();
}

bool Papyrus::groupsCount() const
{
    return papyrus_settings->value("General/GroupsCount",true).toBool();
}

void Papyrus::setAllPaper(bool stt)
{
    papyrus_settings->setValue("General/AllPaper",stt);
    emit allPaperChanged();
}

bool Papyrus::allPaper() const
{
    return papyrus_settings->value("General/AllPaper",true).toBool();
}

void Papyrus::setCanvasHelperFirstTime(bool stt)
{
    papyrus_settings->setValue("General/CanvasHelperFirstTime",stt);
    emit canvasHelperFirstTimeChanged();
}

bool Papyrus::canvasHelperFirstTime() const
{
    return papyrus_settings->value("General/CanvasHelperFirstTime",true).toBool();
}

void Papyrus::setKeyboardPredicative(bool stt)
{
    papyrus_settings->setValue("General/KeyboardPredicative",stt);
    emit keyboardPredicativeChanged();
}

bool Papyrus::keyboardPredicative() const
{
    return papyrus_settings->value("General/KeyboardPredicative",true).toBool();
}

void Papyrus::setPositioning(bool stt)
{
    papyrus_settings->setValue("General/positioning",stt);
    emit positioningChanged();
}

bool Papyrus::positioning() const
{
    return papyrus_settings->value("General/positioning",true).toBool();
}

void Papyrus::setWeatherActive(bool stt)
{
    papyrus_settings->setValue("General/weatherActive",stt);
    emit weatherActiveChanged();
}

bool Papyrus::weatherActive() const
{
    return papyrus_settings->value("General/weatherActive",true).toBool();
}

void Papyrus::setMapMode(Papyrus::MapMode map)
{
    if( p->map_mode == map )
        return;

    p->map_mode = map;
    papyrus_settings->setValue("General/mapMode",static_cast<int>(map));
    emit mapModeChanged();
}

Papyrus::MapMode Papyrus::mapMode() const
{
    return p->map_mode;
}

void Papyrus::setTitleFont(const QFont &fnt)
{
    papyrus_settings->setValue("General/titleFont", fnt);
    emit titleFontChanged();
}

QFont Papyrus::titleFont() const
{
    QFont font;
    font.setFamily("Droid Kaqaz Sans");
    font.setPointSize(15);

    return papyrus_settings->value("General/titleFont",font).value<QFont>();
}

void Papyrus::setBodyFont(const QFont &fnt)
{
    papyrus_settings->setValue("General/bodyFont", fnt);
    emit bodyFontChanged();
}

QFont Papyrus::bodyFont() const
{
    QFont font;
    font.setFamily("Droid Kaqaz Sans");
    font.setPointSize(10);

    return papyrus_settings->value("General/bodyFont",font).value<QFont>();
}

void Papyrus::setSize(const QSize &size)
{
    if( p->fullscreen )
        return;

#ifdef DESKTOP_DEVICE
    if( !p->desktop_touch_mode )
        papyrus_settings->setValue("UserInterface/sizeClassic", size);
    else
        papyrus_settings->setValue("UserInterface/size", size);
#else
    Q_UNUSED(size)
#endif
}

QSize Papyrus::size() const
{
#ifdef DESKTOP_DEVICE
    if( !p->desktop_touch_mode )
        return papyrus_settings->value("UserInterface/sizeClassic", QSize(1024,640)).value<QSize>();
    else
        return papyrus_settings->value("UserInterface/size",QSize(1024*p->viewer->devices()->density(),600*p->viewer->devices()->density())).value<QSize>();
#else
    return QSize(0,0);
#endif
}

void Papyrus::setDesktopTouchMode(bool stt)
{
    if( p->desktop_touch_mode == stt )
        return;

    p->desktop_touch_mode = stt;

#ifdef Q_OS_MAC
    papyrus_settings->setValue("UserInterface/type", p->desktop_touch_mode);
#endif

    emit desktopTouchModeChanged();
}

bool Papyrus::desktopTouchMode() const
{
    return p->desktop_touch_mode;
}

void Papyrus::setFullscreen(bool stt)
{
    if( p->fullscreen == stt )
        return;

    p->fullscreen = stt;

#ifndef DESKTOP_DEVICE
    papyrus_settings->setValue("UserInterface/fullscreen", p->fullscreen);
#endif

    if( p->fullscreen )
    {
#ifdef DESKTOP_DEVICE
        if( !p->desktop_touch_mode )
            p->viewer_classic->showFullScreen();
        else
#endif
            p->viewer->setFullscreen(true);
    }
    else
    {
#ifdef DESKTOP_DEVICE
        if( !p->desktop_touch_mode )
            p->viewer_classic->showNormal();
        else
#endif
            p->viewer->setFullscreen(false);
    }

    emit fullscreenChanged();
}

bool Papyrus::fullscreen() const
{
    return p->fullscreen;
}

QStringList Papyrus::dirEntryFiles(const QString &path, const QStringList & filters)
{
    QStringList res = QDir(path).entryList( filters, QDir::Files, QDir::Time );
    for( int i=0; i<res.count(); i++ )
    {
        const QString & r = res[i];
        res[i] = path + "/" + r;
    }

    return res;
}

QMultiMap<quint64,QString> findEntryFiles_prev(const QString &path, const QStringList &filters)
{
    QMultiMap<quint64,QString> res_map;

    QStringList dirs = Papyrus::dirEntryDirs( path );
    foreach( const QString & d, dirs )
    {
        const QMultiMap<quint64,QString> & r = findEntryFiles_prev( d, filters );
        res_map.unite(r);
    }

    QStringList files = Papyrus::dirEntryFiles( path, filters );
    foreach( const QString & f, files )
    {
        res_map.insert( QFileInfo(f).created().toMSecsSinceEpoch(), f );
    }

    return res_map;
}

QStringList Papyrus::findEntryFiles(const QString &path, const QStringList &filters)
{
    QStringList res;
    const QStringList & list = findEntryFiles_prev(path,filters).values();
    foreach( const QString & l, list )
        res.prepend(l);

    return res;
}

QStringList Papyrus::dirEntryDirs(const QString &path)
{
    QStringList res = QDir(path).entryList( QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name );
    for( int i=0; i<res.count(); i++ )
    {
        const QString & r = res[i];
        res[i] = path + "/" + r;
    }

    return res;
}

Qt::LayoutDirection Papyrus::languageDirection()
{
    return p->locales.value(currentLanguage()).textDirection();
}

QStringList Papyrus::getOpenFileNames(const QString &title, const QString &filter)
{
#ifdef DESKTOP_DEVICE
    return QFileDialog::getOpenFileNames( 0, title, QString(), filter );
#else
    Q_UNUSED(title)
    Q_UNUSED(filter)
    return QStringList();
#endif
}

QByteArray Papyrus::encrypt(const QByteArray &ar, const QString &password)
{
    QByteArray res;
    QSharedPointer<SimpleQtCryptor::Key> gKey = QSharedPointer<SimpleQtCryptor::Key>(new SimpleQtCryptor::Key(password));
    SimpleQtCryptor::Encryptor enc( gKey, SimpleQtCryptor::SERPENT_32, SimpleQtCryptor::ModeCFB, SimpleQtCryptor::NoChecksum );
    enc.encrypt( ar, res, true );
    return res;
}

QByteArray Papyrus::decrypt(const QByteArray &ar, const QString &password)
{
    QByteArray res;
    QSharedPointer<SimpleQtCryptor::Key> gKey = QSharedPointer<SimpleQtCryptor::Key>(new SimpleQtCryptor::Key(password));
    SimpleQtCryptor::Decryptor dec( gKey, SimpleQtCryptor::SERPENT_32, SimpleQtCryptor::ModeCFB );
    dec.decrypt( ar, res, true );
    return res;
}

void Papyrus::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == p->lock_timer )
    {
#ifdef DESKTOP_DEVICE
        if( !p->desktop_touch_mode )
            p->viewer_classic->lock();
        else
#endif
            QMetaObject::invokeMethod( p->viewer->rootObject(), "lock" );

        killTimer( p->lock_timer );
        p->lock_timer = 0;
    }
}

bool Papyrus::eventFilter(QObject *o, QEvent *e)
{
    if( o == p->viewer )
    {
        switch( static_cast<int>(e->type()) )
        {
        case QEvent::Close:
            if( p->close_blocker )
            {
                static_cast<QCloseEvent*>(e)->ignore();
                emit backRequest();
            }
            else
            {
                cleanCache();
                static_cast<QCloseEvent*>(e)->accept();
            }

            return false;
            break;
        }
    }

    return QObject::eventFilter(o,e);
}

Papyrus::~Papyrus()
{
//    delete p->viewer;
    delete p->calendar;
    delete p;
}

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

#define BEGIN_FNC \
    if(p->started) return; \
    BEGIN_FNC_DEBUG \
    p->started = true; \
    emit started(this);

#define END_FNC { \
    p->started = false; \
    emit finished(this); \
    END_FNC_DEBUG \
    return; }

#define NAME_OF(METHOD) #METHOD

#define WRITE_REPEAT_FAILED( FILE, DATA ) { \
    if( DATA.isEmpty() ) \
        END_FNC \
    qint64 res = -1; \
    bool flush = false; \
    int counter = 0; \
    do { \
        if( FILE.open(QDropboxFile::WriteOnly) ) { \
            res = FILE.write(DATA); \
            flush = FILE.flush(); \
            FILE.close(); \
            if( !flush ) \
                res = -1; \
        } \
        counter++; \
    } while( res == -1 && counter <= 5 ); \
    if( res == -1 && counter > 5 ) { \
        smart_io_dbox_fatal = true; \
        END_FNC } \
    }

#define READ_REPEAT_FAILED( FILE, RES, FAILED_CMD ) \
    QByteArray RES; { \
    int counter = 0; \
    do { \
        if( FILE.open(QDropboxFile::ReadOnly) ) {\
            RES = FILE.readAll(); \
            FILE.close(); \
        } \
        counter++; \
    } while( RES.isEmpty() && counter <= 5 ); \
    if( RES.isEmpty() ) { \
        FAILED_CMD; \
        smart_io_dbox_fatal = true; \
        END_FNC \
    } \
    }

#define CHECK_DATA( DATA, RES ) \
    const QByteArray & RES = DATA; \
    if( RES.isEmpty() ) \
        END_FNC

#define PAPER_DATA_HEADER  QString("PapyrusPaper")
#define PAPER_DATA_VERSION QString("1.3")
#define GROUP_DATA_HEADER  QString("PapyrusGroups")
#define GROUP_DATA_VERSION QString("1.0")

#define CALL_CORE_VOID( METHOD ) AsemanTools::call(p->core, NAME_OF(METHOD), Qt::QueuedConnection );
#define CALL_CORE( METHOD, ... ) AsemanTools::call(p->core, NAME_OF(METHOD), Qt::QueuedConnection, __VA_ARGS__ );

#define ALLOC_DBOX(NAME) \
    QDropbox NAME( DROPBOX_APP_KEY, DROPBOX_APP_SECRET, QDropbox::Plaintext, "api.dropbox.com", this ); \
    NAME.setAuthMethod(QDropbox::Plaintext); \
    NAME.setToken(p->token); \
    NAME.setTokenSecret(p->tokenSecret);

#define ALLOC_DBOX_FILE(FILE,PATH) \
    ALLOC_DBOX(dbox) \
    QDropboxFile FILE(PATH,&dbox);

#include "smartiodboxsingle.h"
#include "papyrus.h"
#include "database.h"
#include "asemantools/asemantools.h"
#include "papyrusmacros.h"
#include "qdropbox.h"
#include "qdropboxfile.h"
#include "qdropboxfileinfo.h"
#include "simpleqtcryptor/simpleqtcryptor.h"

#include <QFileInfo>
#include <QFile>
#include <QCoreApplication>
#include <QThread>
#include <QEventLoop>
#include <QMutex>
#include <QDebug>
#include <QBuffer>
#include <QDataStream>
#include <QGeoCoordinate>

bool smart_io_dbox_fatal = false;

class SmartIODBoxSinglePrivate
{
public:
    QString password;

    bool started;

    QThread *thread;
    QEventLoop *loop;

    QString token;
    QString tokenSecret;

    SmartIODBoxSingleCore *core;

    QByteArray cache;
    QMutex cache_mutex;
};

SmartIODBoxSingle::SmartIODBoxSingle(QObject *parent) :
    QObject(parent)
{
    p = new SmartIODBoxSinglePrivate;
    p->started = false;

    p->loop = new QEventLoop(this);

    p->core = new SmartIODBoxSingleCore();
    p->core->moveToThread( Papyrus::database()->thread() );

    p->thread = new QThread();
    moveToThread(p->thread);

    p->thread->start();

    connect( p->thread, SIGNAL(destroyed())           , SLOT(deleteLater())        , Qt::QueuedConnection );
    connect( p->core  , SIGNAL(paperIsReady(QByteArray)) , SLOT(cacheIsReady(QByteArray)), Qt::QueuedConnection );
    connect( p->core  , SIGNAL(groupsIsReady(QByteArray)), SLOT(cacheIsReady(QByteArray)), Qt::QueuedConnection );
    connect( p->core  , SIGNAL(finished())            , SLOT(coreFinished())       , Qt::QueuedConnection );
}

void SmartIODBoxSingle::setToken(const QString &t)
{
    p->token = t;
}

QString SmartIODBoxSingle::token() const
{
    return p->token;
}

void SmartIODBoxSingle::setTokenSecret(const QString &s)
{
    p->tokenSecret = s;
}

QString SmartIODBoxSingle::tokenSecret() const
{
    return p->tokenSecret;
}

void SmartIODBoxSingle::setPassword(const QString &pass)
{
    if( p->password == pass )
        return;

    p->password = pass;
    emit passwordChanged();
}

QString SmartIODBoxSingle::password() const
{
    return p->password;
}

void SmartIODBoxSingle::pushFile(const QString &file, const QString & dest)
{
    BEGIN_FNC
    QFile src(file);
    if( !src.open(QFile::ReadOnly) )
        END_FNC

    ALLOC_DBOX_FILE(dst,dest);
    CHECK_DATA(src.readAll(),data);

    const QByteArray & _d = encryptData(data);
    WRITE_REPEAT_FAILED( dst, _d )

    src.close();
    END_FNC
}

void SmartIODBoxSingle::fetchFile(const QString &path, const QString & dest)
{
    BEGIN_FNC

    QFile ofile(dest);
    ofile.remove();
    if( !ofile.open(QFile::WriteOnly) )
        END_FNC

    ALLOC_DBOX_FILE(file,path)
    READ_REPEAT_FAILED( file, _d, ofile.close();ofile.remove(); );
    CHECK_DATA( decryptData(_d), data );

    ofile.write(data);
    ofile.flush();
    ofile.close();
    END_FNC
}

void SmartIODBoxSingle::pushPaper(const QString &uuid, qint64 revision, const QString & dest)
{
    BEGIN_FNC
    QString gid = uuid;
    gid.remove("{").remove("}");

    CALL_CORE(requestPaperToSync, uuid );
    p->loop->exec();

    const QByteArray cached_data = p->cache;
    if( cached_data.isEmpty() )
        END_FNC;

    const QByteArray & _d = encryptData(cached_data);

    ALLOC_DBOX_FILE(file,dest)
    WRITE_REPEAT_FAILED( file, _d )

    CALL_CORE(requestPaperToSync, uuid );
    p->loop->exec();

    if( p->cache != cached_data )
        END_FNC

    CALL_CORE( paperPushed, uuid, revision );
    p->loop->exec();

    emit revisionChanged( gid, revision );
    END_FNC
}

void SmartIODBoxSingle::fetchPaper(const QString &uuid, qint64 revision, const QString & path)
{
    BEGIN_FNC
    QString gid = uuid;
    gid.remove("{").remove("}");

    ALLOC_DBOX_FILE(file,path)
    READ_REPEAT_FAILED( file, _d, (void)path )

    const QByteArray & data = decryptData(_d);
    if( data.isEmpty() )
        END_FNC

    CALL_CORE( paperFetched, uuid, data, revision );
    p->loop->exec();

    emit revisionChanged( gid, revision );
    END_FNC
}

void SmartIODBoxSingle::pushGroups(const QString & path, qint64 revision)
{
    BEGIN_FNC

    CALL_CORE_VOID( requestGroupsToSync );
    p->loop->exec();

    if( p->cache.isEmpty() )
        END_FNC

    const QByteArray & _d = encryptData(p->cache);

    ALLOC_DBOX_FILE(file,path);
    WRITE_REPEAT_FAILED( file, _d );

    CALL_CORE( groupsPushed, revision );
    p->loop->exec();

    emit revisionChanged(GROUPS_SYNC_KEY,revision);
    END_FNC
}

void SmartIODBoxSingle::fetchGroups(const QString & path, qint64 revision, qint64 current_revision)
{
    BEGIN_FNC

    ALLOC_DBOX_FILE(file,path);
    READ_REPEAT_FAILED( file, _d, (void)path )

    const QByteArray & data = decryptData(_d);
    if( data.isEmpty() )
        END_FNC

    CALL_CORE( groupsFetched, data, revision, current_revision );
    p->loop->exec();

    emit revisionChanged( GROUPS_SYNC_KEY, revision );
    END_FNC
}

void SmartIODBoxSingle::setDeleted(const QString &path)
{
    BEGIN_FNC

    ALLOC_DBOX_FILE(file,path);
    WRITE_REPEAT_FAILED( file, QByteArray("DELETED") )

    emit revisionChanged(path,-2);
    END_FNC
}

void SmartIODBoxSingle::close()
{
    BEGIN_FNC_DEBUG
    p->thread->quit();
    p->thread->wait();
    p->thread->deleteLater();
    END_FNC_DEBUG
}

QString SmartIODBoxSingle::cache() const
{
    BEGIN_FNC_DEBUG
    p->cache_mutex.lock();
    QString result = p->cache;
    p->cache_mutex.unlock();
    END_FNC_DEBUG
    return result;
}

bool SmartIODBoxSingle::fatalError()
{
    return smart_io_dbox_fatal;
}

void SmartIODBoxSingle::cacheIsReady(const QByteArray &data)
{
    BEGIN_FNC_DEBUG
    p->cache_mutex.lock();
    p->cache = data;
    p->cache_mutex.unlock();
    END_FNC_DEBUG
}

void SmartIODBoxSingle::coreFinished()
{
    BEGIN_FNC_DEBUG
    p->loop->exit();
    END_FNC_DEBUG
}

QByteArray SmartIODBoxSingle::encryptData(const QByteArray &data)
{
    BEGIN_FNC_DEBUG
    QSharedPointer<SimpleQtCryptor::Key> gKey = QSharedPointer<SimpleQtCryptor::Key>(new SimpleQtCryptor::Key(p->password));
    SimpleQtCryptor::Encryptor enc( gKey, SimpleQtCryptor::SERPENT_32, SimpleQtCryptor::ModeCFB, SimpleQtCryptor::NoChecksum );

    QByteArray enc_new_data;
    enc.encrypt( data, enc_new_data, true );

    BEGIN_FNC_DEBUG
    return enc_new_data;
}

QByteArray SmartIODBoxSingle::decryptData(const QByteArray &sdata)
{
    BEGIN_FNC_DEBUG
    QSharedPointer<SimpleQtCryptor::Key> gKey = QSharedPointer<SimpleQtCryptor::Key>(new SimpleQtCryptor::Key(p->password));
    SimpleQtCryptor::Decryptor dec( gKey, SimpleQtCryptor::SERPENT_32, SimpleQtCryptor::ModeCFB );
    QByteArray enc_code_dec;
    if( dec.decrypt(sdata,enc_code_dec,true) == SimpleQtCryptor::ErrorInvalidKey )
    {
        END_FNC_DEBUG
        return enc_code_dec;
    }

    END_FNC_DEBUG
    return enc_code_dec;
}

SmartIODBoxSingle::~SmartIODBoxSingle()
{
    BEGIN_FNC_DEBUG
    p->core->deleteLater();
    END_FNC_DEBUG
    delete p;
}

void SmartIODBoxSingleCore::requestPaperToSync(const QString &uuid)
{
    BEGIN_FNC_DEBUG
    Database *db = Papyrus::database();
    int paperId = db->paperUuidId(uuid);
    if( paperId == -1 )
    {
        emit finished();
        END_FNC_DEBUG
        return;
    }

    QByteArray d;
    QBuffer buffer(&d);
    buffer.open(QBuffer::WriteOnly);
    QDataStream stream(&buffer);

    stream << PAPER_DATA_HEADER;
    stream << PAPER_DATA_VERSION;
    stream << db->paperTitle(paperId);
    stream << db->paperLocation(paperId);
    stream << db->paperCreatedDate(paperId).toString();
    stream << db->paperModifiedDate(paperId).toString();
    stream << db->groupUuid( db->paperGroup(paperId) );
    stream << db->paperFiles(paperId);
    stream << db->paperText(paperId);
    stream << db->paperType(paperId);
    stream << db->paperWeather(paperId);
    stream << db->paperTemperature(paperId);

    buffer.close();

    emit paperIsReady(d);
    emit finished();
    END_FNC_DEBUG
}

void SmartIODBoxSingleCore::requestGroupsToSync()
{
    BEGIN_FNC_DEBUG
    Database *db = Papyrus::database();

    QByteArray data;
    QBuffer mainBuffer(&data);
    mainBuffer.open(QBuffer::WriteOnly);
    QDataStream mainStream(&mainBuffer);
    mainStream << GROUP_DATA_HEADER;
    mainStream << GROUP_DATA_VERSION;

    const QList<int> & groups = db->groups();
    foreach( int gr, groups )
    {
        QByteArray d;
        QBuffer buffer(&d);
        buffer.open(QBuffer::WriteOnly);

        QDataStream stream(&buffer);
        stream << db->groupUuid(gr);
        stream << db->groupColor(gr).name();
        stream << db->groupName(gr);

        buffer.close();
        mainStream << d;
    }

    mainBuffer.close();

    emit groupsIsReady(data);
    emit finished();
    END_FNC_DEBUG
}

void SmartIODBoxSingleCore::paperPushed(const QString &id, quint64 revision)
{
    BEGIN_FNC_DEBUG
    Database *db = Papyrus::database();
    db->setSignalBlocker(true);
    db->setRevision(id,revision);
    db->setSignalBlocker(false);
    emit finished();
    END_FNC_DEBUG
}

void SmartIODBoxSingleCore::paperFetched(const QString &uuid, const QByteArray &d_const, quint64 revision)
{
    BEGIN_FNC_DEBUG
    if( d_const.isEmpty() )
    {
        emit finished();
        END_FNC_DEBUG
        return;
    }

    Database *db = Papyrus::database();
    db->setSignalBlocker(true);

    QByteArray d = d_const;
    QBuffer buffer(&d);
    buffer.open(QBuffer::ReadOnly);
    QDataStream stream(&buffer);

    QString tmp_string;

    QString header;
    QString version;
    QString title;
    QGeoCoordinate location;
    QString date;
    QString mdate;
    QString group;
    QStringList files;
    QString body;
    int type = Enums::Normal;
    QString weather;
    int temperature = 0;

    stream >> header;
    stream >> version;
    stream >> title;
    if( version == "1.0" )
        stream >> tmp_string;
    else
        stream >> location;
    stream >> date;
    stream >> mdate;
    stream >> group;
    stream >> files;
    stream >> body;
    if( version.toDouble() > 1.1 )
    {
        stream >> type;
    }
    if( version.toDouble() > 1.2 )
    {
        stream >> weather;
        stream >> temperature;
    }

    db->setPaper( uuid, title, body, group, date, location, type, weather, temperature );
    db->setRevision( uuid, revision );

    int paperId = db->paperUuidId(uuid);
    for( int i=files.count()-1; i>=0 ; i-- )
        db->addFileToPaper( paperId, files[i] );

    db->setSignalBlocker(false);
    emit finished();
    END_FNC_DEBUG
}

void SmartIODBoxSingleCore::groupsPushed(quint64 revision)
{
    BEGIN_FNC_DEBUG
    Database *db = Papyrus::database();
    db->setSignalBlocker(true);
    db->setRevision( GROUPS_SYNC_KEY, revision );
    db->setSignalBlocker(false);
    emit finished();
    END_FNC_DEBUG
}

void SmartIODBoxSingleCore::groupsFetched(const QByteArray &data, quint64 revision, qint64 current_revision)
{
    BEGIN_FNC_DEBUG
    if( data.isEmpty() )
    {
        emit finished();
        END_FNC_DEBUG
        return;
    }

    Database *db = Papyrus::database();
    db->setSignalBlocker(true);

    QByteArray mainData = data;
    QBuffer mainBuffer(&mainData);
    mainBuffer.open(QBuffer::ReadOnly);
    QDataStream mainStream(&mainBuffer);

    QString header;
    QString version;

    mainStream >> header;
    mainStream >> version;

    while( !mainStream.atEnd() )
    {
        QByteArray record;
        mainStream >> record;

        QBuffer buffer(&record);
        buffer.open(QBuffer::ReadOnly);
        QDataStream stream(&buffer);

        QString uuid;
        QString color;
        QString name;

        stream >> uuid;
        stream >> color;
        stream >> name;

        db->setGroup( uuid, name, color );
        buffer.close();
    }

    mainBuffer.close();

    db->setRevision( GROUPS_SYNC_KEY, revision );
    if( current_revision == -1 )
        db->setRevision( GROUPS_SYNC_KEY, -1 );

    db->setSignalBlocker(false);
    emit finished();
    END_FNC_DEBUG
}

void SmartIODBoxSingleCore::paperDeleted(const QString &id)
{
    BEGIN_FNC_DEBUG
    Database *db = Papyrus::database();
    int paperId = db->paperUuidId(id);
    if( paperId == -1 )
    {
        emit finished();
        END_FNC_DEBUG
        return;
    }

    db->setSignalBlocker(true);
    db->deletePaper( paperId );
    db->setSignalBlocker(false);

    emit finished();
    END_FNC_DEBUG
}

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

#define ROOT_FOLDER      "/sandbox/"
#define SYNC_VERSION     "2.0"
#define PAPERS_FOLDER    ROOT_FOLDER SYNC_VERSION "/papers/"
#define ATTACHS_FOLDER   ROOT_FOLDER SYNC_VERSION "/attachs/"
#define GROUPS_FILE      ROOT_FOLDER SYNC_VERSION "/groups"
#define REVISIONS_FILE   ROOT_FOLDER SYNC_VERSION "/revisions"
#define MAXIMUM_UPLOAD   10
#define FILE_MAX_SIZE_MB 20
#define UPDATE_KEY       "DBOX_UPDATE"
#define DBOX_CLOSE(DBFL) DBFL.flush(); DBFL.close();

#define ALLOC_DBOX(NAME) \
    QDropbox NAME( DROPBOX_APP_KEY, DROPBOX_APP_SECRET, QDropbox::Plaintext, "api.dropbox.com", this ); \
    NAME.setAuthMethod(QDropbox::Plaintext); \
    NAME.setToken(p->settings->value(TOKEN_KEY).toString()); \
    NAME.setTokenSecret(p->settings->value(TOKEN_SECRET).toString());

#include "papyrusdropbox.h"
#include "qdropbox.h"
#include "qdropboxfile.h"
#include "smartiodboxsingle.h"
#include "database.h"
#include "smartiodbox.h"
#include "papyrusmacros.h"
#include "papyrus.h"
#include "simpleqtcryptor/simpleqtcryptor.h"

#include <QDesktopServices>
#include <QEventLoop>
#include <QTimer>
#include <QDir>
#include <QUuid>
#include <QMutex>
#include <QQueue>
#include <QSettings>
#include <QTimerEvent>
#include <QDataStream>
#include <QDebug>

class PapyrusDropBoxPrivate
{
public:
    QDropbox *dbox;
    QEventLoop *loop;
    SmartIODBox *smartio;

    bool connected;
    QString master_password;
    bool password_setted;

    QMutex mutex;

    SyncItemHash syncHash;

    int refresh_timer;
    QHash<QString,qint64> revisions;

    bool reupdate_flag;
    bool rsvn_fetched;
    bool token;
    bool fileSyncing;

    QSet<QString> fnc_recall;
    QSet<QString> on_fnc_call;
    QQueue<QString> fnc_pend_update;

    QSettings *settings;
};

PapyrusDropBox::PapyrusDropBox(QObject *parent) :
    QObject(parent)
{
    p = new PapyrusDropBoxPrivate;
    p->dbox = 0;
    p->loop = 0;
    p->smartio = 0;
    p->refresh_timer = 0;
    p->connected = false;
    p->rsvn_fetched = false;
    p->reupdate_flag = false;
    p->token = false;
    p->fileSyncing = true;
    p->password_setted = false;

    p->settings = new QSettings(CONFIG_PATH,QSettings::IniFormat);
    checkToken();
}

bool PapyrusDropBox::connected() const
{
    return p->connected;
}

bool PapyrusDropBox::tokenAvailable() const
{
    return p->token;
}

void PapyrusDropBox::setFileSyncing(bool stt)
{
    BEGIN_FNC_DEBUG
    p->fileSyncing = stt;
    END_FNC_DEBUG
}

bool PapyrusDropBox::fileSyncing() const
{
    return p->fileSyncing;
}

void PapyrusDropBox::setLocalSyncHash(const SyncItemHash &hash)
{
    BEGIN_FNC_DEBUG
    p->mutex.lock();
    p->syncHash = hash;
    p->mutex.unlock();
    END_FNC_DEBUG
}

SyncItemHash PapyrusDropBox::localSyncHash() const
{
    BEGIN_FNC_DEBUG
    p->mutex.lock();
    SyncItemHash res = p->syncHash;
    p->mutex.unlock();

    END_FNC_DEBUG
    return res;
}

void PapyrusDropBox::localListUpdated()
{
    if( !p->password_setted )
        return;
    if( !beginPush(UPDATE_KEY) )
        return;

    BEGIN_FNC_DEBUG
    if( p->on_fnc_call.count() != 1 )
    {
        refresh();
        endPush(UPDATE_KEY);
        END_FNC_DEBUG
        return;
    }

    ALLOC_DBOX(papersDbox);
    ALLOC_DBOX(filesDbox);

    const QDropboxFileInfo &papersInf = papersDbox.requestMetadataAndWait(PAPERS_FOLDER);
    const QDropboxFileInfo &filesInf = filesDbox.requestMetadataAndWait(ATTACHS_FOLDER);

    if( !papersInf.isDir() || !filesInf.isDir() )
    {
        refresh();
        if( SmartIODBoxSingle::fatalError() )
        {
            endPush(UPDATE_KEY);
            END_FNC_DEBUG
            return;
        }
    }

    emit syncStarted();

    const QList<QDropboxFileInfo> & papers = papersInf.contents();
    const QList<QDropboxFileInfo> & files  = filesInf.contents();

    const SyncItemHash & syncHash = localSyncHash();
    if( syncHash.contains(GROUPS_SYNC_KEY) )
    {
        const SyncItem & item = syncHash.value(GROUPS_SYNC_KEY);
        qint64 revision = fetchRevision(GROUPS_SYNC_KEY);
        if( revision > item.revision && (item.revision != -1 || item.last_revision == -1) )
        {
            p->smartio->fetchGroups(GROUPS_FILE, revision, item.revision );
        }
    }

    QHash<QString,QDropboxFileInfo> papersIndexes;
    foreach( const QDropboxFileInfo & inf, papers )
    {
        const QString & id = "{" + QFileInfo(inf.path()).fileName() + "}";
        papersIndexes[id] = inf;

        qint64 revision = fetchRevision(inf.path());

        SyncItem item = syncHash.value(id);
        if( revision == -2 )
        {
            if( item.revision != -2 )
                emit paperDeleted(item.id);
            continue;
        }
        if( item.id == id && item.revision == revision )
            continue;
        if( !item.id.isEmpty() && item.revision == -1 )
            continue;

        p->smartio->fetchPaper(id, revision, ROOT_FOLDER+inf.path());
    }

    QHash<QString,QDropboxFileInfo> filesIndexes;
    const QStringList & local_files = QDir(Papyrus::instance()->repositoryPath()).entryList(QDir::Files);
    foreach( const QDropboxFileInfo & inf, files )
    {
        const QString & fileName = QFileInfo(inf.path()).fileName();
        filesIndexes[fileName] = inf;

        if( !p->fileSyncing )
            continue;
        if( dboxFileIsDeleted(ROOT_FOLDER+inf.path()) )
        {
            if( !fileIsDeleted(Papyrus::instance()->repositoryPath() + "/" + fileName) )
                setFileDeleted(Papyrus::instance()->repositoryPath() + "/" + fileName);
            continue;
        }
        if( local_files.contains(fileName) )
            continue;
        if( inf.bytes()/(1024*1024) >= FILE_MAX_SIZE_MB )
            continue;

        p->smartio->fetchFile( ROOT_FOLDER+inf.path(), Papyrus::instance()->repositoryPath() + "/" + fileName );
    }

    foreach( const QString & lfile, local_files )
    {
        const QString & path = QString(Papyrus::instance()->repositoryPath() + "/" + lfile);
        if( !p->fileSyncing )
            continue;
        if( fileIsDeleted(path) )
        {
            if( !dboxFileIsDeleted(ATTACHS_FOLDER + lfile) )
                p->smartio->setDeleted(ATTACHS_FOLDER + lfile);
            continue;
        }
        if( filesIndexes.contains(lfile) )
            continue;
        if( QFileInfo(path).size()/(1024*1024) >= FILE_MAX_SIZE_MB )
            continue;

        p->smartio->pushFile( path, ATTACHS_FOLDER + lfile );
    }

    QHashIterator<QString,SyncItem> i(syncHash);
    while( i.hasNext() )
    {
        i.next();

        const SyncItem & item = i.value();
        if( item.id.isEmpty() )
            continue;

        if( item.id[0] == '{' ) // It's Paper
        {
            QDropboxFileInfo inf = papersIndexes.value(item.id);
            qint64 revision = fetchRevision(inf.path());
            if( revision == -2 )
                continue;
            if( revision >= item.revision && item.revision >= 0 && papersIndexes.contains(item.id) )
                continue;
            if( item.revision == -2 )
            {
                p->smartio->setDeleted(PAPERS_FOLDER+QString(item.id).remove("{").remove("}"));
                continue;
            }

            p->smartio->pushPaper(item.id,revision+1,PAPERS_FOLDER+QString(item.id).remove("{").remove("}"));
        }
        else
        if( item.id == GROUPS_SYNC_KEY ) // It's Groups list
        {
            qint64 revision = fetchRevision(GROUPS_SYNC_KEY);
            if( revision >= item.revision && item.revision != -1 )
                continue;
            if( item.last_revision == -1 && item.revision == -1 && revision != -1 )
                continue;

            p->smartio->pushGroups(GROUPS_FILE,revision+1);
        }
    }

    p->smartio->start();
    if( !p->smartio->isActive() )
        update_finished();

    END_FNC_DEBUG
}

void PapyrusDropBox::update_finished()
{
    BEGIN_FNC_DEBUG
    emit syncFinished();
    if( !endPush(UPDATE_KEY) )
        emit refreshRequest();
    END_FNC_DEBUG
}

void PapyrusDropBox::connectDropbox(const QString &password)
{
    BEGIN_FNC_DEBUG
    if( p->connected )
    {
        p->password_setted = true;
        p->master_password = password;
        p->smartio->setPassword(p->master_password);
        refresh();
        END_FNC_DEBUG
        return;
    }

    if(!p->dbox->requestTokenAndWait())
    {
        emit getTokenFailed();
        END_FNC_DEBUG
        return;
    }

    if(!p->dbox->requestAccessTokenAndWait())
    {
        int i = 0;
        for(;i<3; ++i)
        {
            if(p->dbox->error() != QDropbox::TokenExpired)
                break;

            authorizeApplication();
        }

        if(i>3)
        {
            emit authorizeFailed();
            END_FNC_DEBUG
            return;
        }

        if(p->dbox->error() != QDropbox::NoError)
        {
            emit accessError( p->dbox->errorString() );
            END_FNC_DEBUG
            return;
        }
    }

    p->settings->setValue(TOKEN_KEY,p->dbox->token());
    p->settings->setValue(TOKEN_SECRET,p->dbox->tokenSecret());
    p->settings->sync();
    p->smartio->setToken(p->dbox->token());
    p->smartio->setTokenSecret(p->dbox->tokenSecret());
    checkToken();

    p->connected = true;
    refresh();
    emit connectedChanged();
    END_FNC_DEBUG
}

void PapyrusDropBox::disconnectDropbox()
{
    BEGIN_FNC_DEBUG
    p->settings->remove(TOKEN_KEY);
    p->settings->remove(TOKEN_SECRET);
    p->settings->sync();
    p->dbox->setToken(QString());
    p->dbox->setTokenSecret(QString());
    p->smartio->setToken(p->dbox->token());
    p->smartio->setTokenSecret(p->dbox->tokenSecret());
    p->connected = false;
    emit connectedChanged();
    END_FNC_DEBUG
}

void PapyrusDropBox::initialize()
{
    BEGIN_FNC_DEBUG
    if(p->dbox )
        return;

    p->loop = new QEventLoop(this);

    p->dbox = new QDropbox( DROPBOX_APP_KEY, DROPBOX_APP_SECRET, QDropbox::Plaintext, "api.dropbox.com", this );
    p->dbox->setAuthMethod(QDropbox::Plaintext);

    p->smartio = new SmartIODBox(this);

    p->connected = false;

    if(!p->settings->value(TOKEN_KEY).toString().isEmpty())
    {
        p->dbox->setToken(p->settings->value(TOKEN_KEY).toString());
        p->dbox->setTokenSecret(p->settings->value(TOKEN_SECRET).toString());
        p->smartio->setToken(p->dbox->token());
        p->smartio->setTokenSecret(p->dbox->tokenSecret());
        p->connected = true;
        checkToken();
    }

    connect( p->smartio, SIGNAL(revisionChanged(QString,qint64)), SLOT(setRevision(QString,qint64)), Qt::QueuedConnection );
    connect( p->smartio, SIGNAL(progressChanged(qreal))         , SIGNAL(syncProgress(qreal))      , Qt::QueuedConnection );
    connect( p->smartio, SIGNAL(progressFinished())             , SLOT(update_finished())          , Qt::QueuedConnection );

    emit connectedChanged();
    END_FNC_DEBUG
}

void PapyrusDropBox::refresh()
{
    BEGIN_FNC_DEBUG
    if( p->refresh_timer )
        killTimer(p->refresh_timer);

    p->refresh_timer = startTimer(5000);
    END_FNC_DEBUG
}

void PapyrusDropBox::refreshForce()
{
    BEGIN_FNC_DEBUG
    localListUpdated();
    END_FNC_DEBUG
}

QByteArray PapyrusDropBox::encryptData(const QByteArray &data)
{
    BEGIN_FNC_DEBUG
    QSharedPointer<SimpleQtCryptor::Key> gKey = QSharedPointer<SimpleQtCryptor::Key>(new SimpleQtCryptor::Key(p->master_password));
    SimpleQtCryptor::Encryptor enc( gKey, SimpleQtCryptor::SERPENT_32, SimpleQtCryptor::ModeCFB, SimpleQtCryptor::NoChecksum );

    QByteArray enc_new_data;
    enc.encrypt( data, enc_new_data, true );

    END_FNC_DEBUG
    return enc_new_data;
}

QByteArray PapyrusDropBox::decryptData(const QByteArray &sdata)
{
    BEGIN_FNC_DEBUG
    QSharedPointer<SimpleQtCryptor::Key> gKey = QSharedPointer<SimpleQtCryptor::Key>(new SimpleQtCryptor::Key(p->master_password));
    SimpleQtCryptor::Decryptor dec( gKey, SimpleQtCryptor::SERPENT_32, SimpleQtCryptor::ModeCFB );
    QByteArray enc_code_dec;
    if( dec.decrypt(sdata,enc_code_dec,true) == SimpleQtCryptor::ErrorInvalidKey )
        return enc_code_dec;

    END_FNC_DEBUG
    return enc_code_dec;
}

bool PapyrusDropBox::fileIsDeleted(const QString &path)
{
    QFile file(path);
    if( !file.open(QFile::ReadOnly) )
        return false;

    const QString data = file.read(10).trimmed();
    file.close();

    if( data == "DELETED" )
        return true;
    else
        return false;
}

void PapyrusDropBox::setFileDeleted(const QString &path)
{
    QFile file(path);
    file.remove();

    if( !file.open(QFile::WriteOnly) )
        return;

    file.write("DELETED");
    file.flush();
    file.close();
}

bool PapyrusDropBox::dboxFileIsDeleted(const QString &path)
{
    return fetchRevision(path) == -2;
}

qint64 PapyrusDropBox::fetchRevision(const QString &path)
{
    fetchRevisions();
    if( path.isEmpty() )
        return -1;

    const QString & id = "{" + QFileInfo(path).fileName() + "}";
    if( p->revisions.contains(id) )
        return p->revisions[id];
    else
        return -1;
}

void PapyrusDropBox::setRevision(const QString &path, qint64 revision)
{
    BEGIN_FNC_DEBUG
    fetchRevisions();
    const QString & id = "{" + QFileInfo(path).fileName() + "}";
    p->revisions[id] = revision;
    pushRevisions();
    END_FNC_DEBUG
}

void PapyrusDropBox::pushRevisions()
{
    if( !beginPush(REVISIONS_FILE) )
        return;

    QDropboxFile rvsn_file(REVISIONS_FILE,p->dbox);
    if( !rvsn_file.open(QDropboxFile::WriteOnly) )
        return;

    BEGIN_FNC_DEBUG
    QString data;
    QHashIterator<QString,qint64> i(p->revisions);
    while(i.hasNext())
    {
        i.next();
        data += i.key() + "=" + QString::number(i.value()) + "\n";
    }

    rvsn_file.write( data.toUtf8() );
    DBOX_CLOSE(rvsn_file)

    if( !endPush(REVISIONS_FILE) )
        pushRevisions();

    END_FNC_DEBUG
}

void PapyrusDropBox::fetchRevisions()
{
    if( p->rsvn_fetched )
        return;

    QDropboxFile file(REVISIONS_FILE,p->dbox);
    if( !file.open(QDropboxFile::ReadOnly) )
        return;

    const QString & data = file.readAll();
    file.flush();
    file.close();

    const QStringList & lines = data.split("\n",QString::SkipEmptyParts);
    foreach( const QString & l, lines )
    {
        int index = l.indexOf("=");
        if( index == -1 )
            continue;

        p->revisions[l.mid(0,index)] = l.mid(index+1).toLongLong();
    }

    p->rsvn_fetched = true;
}

bool PapyrusDropBox::beginPush(const QString &id)
{
    if( p->on_fnc_call.contains(id) )
    {
        p->fnc_recall.insert(id);
        return false;
    }
    else
    {
        p->on_fnc_call.insert(id);
        return true;
    }
}

bool PapyrusDropBox::endPush(const QString &id)
{
    p->on_fnc_call.remove(id);
    if( p->fnc_recall.contains(id) )
    {
        p->fnc_recall.remove(id);
        return false;
    }
    else
        return true;
}

void PapyrusDropBox::authorizeDone()
{
    BEGIN_FNC_DEBUG
    p->loop->exit();
    END_FNC_DEBUG
}

void PapyrusDropBox::authorizeApplication()
{
    BEGIN_FNC_DEBUG
    QDesktopServices::openUrl(p->dbox->authorizeLink());

    emit authorizeRequest();
    p->loop->exec();

    p->dbox->requestAccessTokenAndWait();
    END_FNC_DEBUG
}

void PapyrusDropBox::timerEvent(QTimerEvent *e)
{
    BEGIN_FNC_DEBUG
    if( e->timerId() == p->refresh_timer )
    {
        killTimer(p->refresh_timer);
        p->refresh_timer = 0;

        localListUpdated();
    }
    END_FNC_DEBUG
}

void PapyrusDropBox::checkToken()
{
    bool token = !p->settings->value(TOKEN_KEY).toString().isEmpty() &&
                 !p->settings->value(TOKEN_SECRET).toString().isEmpty();
    if( token == p->token )
        return;

    p->token = token;
    emit tokenAvailableChanged();
}

PapyrusDropBox::~PapyrusDropBox()
{
    delete p;
}

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

#include "papyrussync.h"
#include "asemandeveloper_macros.h"
#include "papyrusmacros.h"
#include "papyrusdropbox.h"
#include "qdropboxfile.h"
#include "database.h"
#include "papyrus.h"
#include "asemantools/asemantools.h"

#include <QThread>
#include <QPointer>

class PapyrusSyncPrivate
{
public:
    PapyrusDropBox *kdbox;
    QPointer<QThread> thread;
    Database *db;

    QString password;
};

PapyrusSync::PapyrusSync(QObject *parent) :
    QObject(parent)
{
    p = new PapyrusSyncPrivate;
    p->db = Papyrus::database();
    p->kdbox = 0;
    p->password = p->db->syncPassword();

    connect( p->db, SIGNAL(paperChanged(int))  , SLOT(refresh()) );
    connect( p->db, SIGNAL(revisionsChanged()) , SLOT(refresh()) );
    connect( p->db, SIGNAL(filesListChanged()) , SLOT(refresh()) );

    reload();
}

bool PapyrusSync::tokenAvailable() const
{
    return p->kdbox->tokenAvailable();
}

void PapyrusSync::setPassword(const QString &pass)
{
    BEGIN_FNC_DEBUG
    QString hash = AsemanTools::passToMd5(pass);
    p->db->setSyncPassword( hash );
    p->password = hash;
    END_FNC_DEBUG
}

QString PapyrusSync::password() const
{
    return p->db->syncPassword();
}

void PapyrusSync::setFileSyncing(bool stt)
{
    BEGIN_FNC_DEBUG
    p->kdbox->setFileSyncing(stt);
    Papyrus::settings()->setValue( "General/fileSyncing", stt );
    emit fileSyncingChanged();
    refresh();
    END_FNC_DEBUG
}

bool PapyrusSync::fileSyncing() const
{
    return p->kdbox->fileSyncing();
}

void PapyrusSync::password_changed(const QString &password)
{
    BEGIN_FNC_DEBUG
    p->db->setSyncPassword( password );
    emit passwordChanged();
    END_FNC_DEBUG
}

void PapyrusSync::connectedChanged()
{
    BEGIN_FNC_DEBUG
    if( p->kdbox->connected() )
    {
        connect( p->db, SIGNAL(paperChanged(int))  , p->kdbox, SLOT(refresh()), Qt::QueuedConnection );
        connect( p->db, SIGNAL(groupsListChanged()), p->kdbox, SLOT(refresh()), Qt::QueuedConnection );

        start();
    }
    else
    {
        disconnect( p->db, SIGNAL(paperChanged(int))  , p->kdbox, SLOT(refresh()) );
        disconnect( p->db, SIGNAL(groupsListChanged()), p->kdbox, SLOT(refresh()) );
    }
    END_FNC_DEBUG
}

void PapyrusSync::refresh()
{
    BEGIN_FNC_DEBUG
    p->kdbox->setLocalSyncHash( p->db->revisions() );
    QMetaObject::invokeMethod( p->kdbox, "refresh", Qt::QueuedConnection );
    END_FNC_DEBUG
}

void PapyrusSync::refreshForce()
{
    BEGIN_FNC_DEBUG
    p->kdbox->setLocalSyncHash( p->db->revisions() );
    QMetaObject::invokeMethod( p->kdbox, "refreshForce", Qt::QueuedConnection );
    END_FNC_DEBUG
}

void PapyrusSync::start()
{
    BEGIN_FNC_DEBUG
    QMetaObject::invokeMethod( p->kdbox, "connectDropbox", Qt::QueuedConnection, Q_ARG(QString,p->password) );
    END_FNC_DEBUG
}

void PapyrusSync::stop()
{
    BEGIN_FNC_DEBUG
    QSettings settings(CONFIG_PATH,QSettings::IniFormat);
    settings.remove(TOKEN_KEY);
    settings.remove(TOKEN_SECRET);
    reload();
    END_FNC_DEBUG
}

void PapyrusSync::reload()
{
    BEGIN_FNC_DEBUG
    if( p->thread )
    {
        p->thread->quit();
        p->thread->wait();
        p->thread->deleteLater();
    }

    p->thread = new QThread(this);

#ifdef FREE_BUILD
    bool file_sync_default = false;
#else
    bool file_sync_default = true;
#endif

    p->kdbox = new PapyrusDropBox();
    p->kdbox->setFileSyncing( Papyrus::settings()->value( "General/fileSyncing", file_sync_default ).toBool() );
    p->kdbox->setLocalSyncHash(p->db->revisions());
    p->kdbox->moveToThread(p->thread);

    connect( p->kdbox, SIGNAL(authorizeRequest())      , SIGNAL(authorizeRequest())     , Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(passwordChanged(QString)), SLOT(password_changed(QString)), Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(paperDeleted(QString))   , SLOT(paperDeleted(QString))    , Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(connectedChanged())      , SLOT(connectedChanged())       , Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(syncStarted())           , SIGNAL(syncStarted())          , Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(syncProgress(qreal))     , SIGNAL(syncProgress(qreal))    , Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(syncFinished())          , SIGNAL(syncFinished())         , Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(tokenAvailableChanged()) , SIGNAL(tokenAvailableChanged()), Qt::QueuedConnection );
    connect( p->kdbox, SIGNAL(refreshRequest())        , SLOT(refreshForce())           , Qt::QueuedConnection );

    connect( p->thread, SIGNAL(finished()), p->kdbox, SLOT(deleteLater()) );

    p->thread->start();
    QMetaObject::invokeMethod( p->kdbox, "initialize", Qt::QueuedConnection );

    emit tokenAvailableChanged();
    END_FNC_DEBUG
}

void PapyrusSync::authorizeDone()
{
    BEGIN_FNC_DEBUG
    QMetaObject::invokeMethod( p->kdbox, "authorizeDone", Qt::QueuedConnection );
    END_FNC_DEBUG
}

void PapyrusSync::paperDeleted(const QString &id)
{
    int paperId = p->db->paperUuidId(id);
    if( paperId == -1 )
        return;

    BEGIN_FNC_DEBUG
    p->db->deletePaper( paperId );
    END_FNC_DEBUG
}

PapyrusSync::~PapyrusSync()
{
    delete p;
}

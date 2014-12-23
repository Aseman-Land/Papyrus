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

#ifndef PAPYRUSDROPBOX_H
#define PAPYRUSDROPBOX_H
#define TOKEN_KEY      "Token/key"
#define TOKEN_SECRET   "Token/Secret"

#include <QObject>
#include <QStringList>
#include <QHash>

#include "databasedatatypes.h"

class PapyrusDropBoxPrivate;
class PapyrusDropBox : public QObject
{
    Q_OBJECT
public:
    PapyrusDropBox(QObject *parent = 0);
    ~PapyrusDropBox();

    bool connected() const;
    bool tokenAvailable() const;

    void setFileSyncing( bool stt );
    bool fileSyncing() const;

    void setLocalSyncHash( const SyncItemHash & hash );
    SyncItemHash localSyncHash() const;

public slots:
    void initialize();
    void refresh();
    void refreshForce();

    void connectDropbox( const QString & password );
    void disconnectDropbox();

    void authorizeDone();

signals:
    void connectedChanged();
    void tokenAvailableChanged();

    void authorizeRequest();
    void getTokenFailed();
    void authorizeFailed();
    void accessError( const QString & error );

    void paperDeleted( const QString & id );
    void passwordChanged( const QString & password );

    void refreshRequest();

    void syncStarted();
    void syncProgress( qreal val );
    void syncFinished();

private slots:
    void authorizeApplication();

    QByteArray encryptData(const QByteArray &data );
    QByteArray decryptData( const QByteArray & sdata );

    bool fileIsDeleted( const QString & path );
    void setFileDeleted( const QString & path );

    bool dboxFileIsDeleted( const QString & path );

    qint64 fetchRevision( const QString & path );
    void setRevision( const QString & path, qint64 revision );

    void pushRevisions();
    void fetchRevisions();

    bool beginPush( const QString & id );
    bool endPush( const QString & id );

    void localListUpdated();
    void update_finished();

protected:
    void timerEvent(QTimerEvent *e);

private:
    void checkToken();

private:
    PapyrusDropBoxPrivate *p;
};

#endif // PAPYRUSDROPBOX_H

/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Kaqaz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Kaqaz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef KAQAZSYNC_H
#define KAQAZSYNC_H

#include <QObject>

class KaqazSyncPrivate;
class KaqazSync : public QObject
{
    Q_PROPERTY(bool tokenAvailable READ tokenAvailable NOTIFY tokenAvailableChanged)
    Q_PROPERTY(bool fileSyncing READ fileSyncing WRITE setFileSyncing NOTIFY fileSyncingChanged)
    Q_PROPERTY(QString password READ password NOTIFY passwordChanged)
    Q_OBJECT
public:
    KaqazSync(QObject *parent = 0);
    ~KaqazSync();

    bool tokenAvailable() const;

    QString password() const;

    void setFileSyncing( bool stt );
    bool fileSyncing() const;

public slots:
    void start();
    void stop();
    void reload();
    void authorizeDone();
    void setPassword( const QString & pass );

    void refresh();
    void refreshForce();

signals:
    void authorizeRequest();

    void syncStarted();
    void syncProgress( qreal val );
    void syncFinished();

    void tokenAvailableChanged();
    void passwordChanged();
    void fileSyncingChanged();

private slots:
    void password_changed( const QString & password );
    void paperDeleted( const QString & id );

    void connectedChanged();

private:
    KaqazSyncPrivate *p;
};

#endif // KAQAZSYNC_H

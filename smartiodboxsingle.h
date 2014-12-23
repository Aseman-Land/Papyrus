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

#ifndef SMARTIODBOXSINGLE_H
#define SMARTIODBOXSINGLE_H

#include <QObject>

class SmartIODBoxSinglePrivate;
class SmartIODBoxSingle : public QObject
{
    Q_OBJECT
public:
    SmartIODBoxSingle(QObject *parent = 0);
    ~SmartIODBoxSingle();

public slots:
    void setToken(const QString & t);
    QString token() const;

    void setTokenSecret(const QString & s);
    QString tokenSecret() const;

    void setPassword( const QString & pass );
    QString password() const;

    void pushFile(const QString &file, const QString &dest);
    void fetchFile(const QString &path , const QString &dest);

    void pushPaper(const QString & uuid, qint64 revision , const QString &dest);
    void fetchPaper(const QString & uuid, qint64 revision, const QString & path);

    void pushGroups( const QString & path, qint64 revision );
    void fetchGroups( const QString & path, qint64 revision, qint64 current_revision );

    void setDeleted(const QString & path );

    void close();
    QString cache() const;

    static bool fatalError();

signals:
    void passwordChanged();

    void started( SmartIODBoxSingle *s );
    void finished( SmartIODBoxSingle *s );

    void revisionChanged( const QString & id, qint64 revision );

private slots:
    void cacheIsReady(const QByteArray &data );
    void coreFinished();

private:
    QByteArray encryptData(const QByteArray &data );
    QByteArray decryptData( const QByteArray & sdata );

private:
    SmartIODBoxSinglePrivate *p;
};

class SmartIODBoxSingleCore: public QObject
{
    Q_OBJECT
    friend class SmartIODBoxSingle;
public:
    SmartIODBoxSingleCore(QObject *parent = 0): QObject(parent){}

public slots:
    void requestPaperToSync( const QString & uuid );
    void requestGroupsToSync();

    void paperPushed(const QString & id, quint64 revision );
    void paperFetched(const QString & id, const QByteArray &data, quint64 revision );

    void groupsPushed( quint64 revision );
    void groupsFetched(const QByteArray &data, quint64 revision, qint64 current_revision );

    void paperDeleted( const QString & id );

signals:
    void paperIsReady( const QByteArray & data );
    void groupsIsReady( const QByteArray & data );
    void finished();
};

#endif // SMARTIODBOXSINGLE_H

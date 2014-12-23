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

#ifndef SMARTIODBOX_H
#define SMARTIODBOX_H

#include <QObject>

class SmartIODBoxSingle;
class SmartIODBoxPrivate;
class SmartIODBox : public QObject
{
    Q_OBJECT
public:
    SmartIODBox(QObject *parent = 0);
    ~SmartIODBox();

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

    qreal progress() const;
    bool isActive() const;

public slots:
    void start();

signals:
    void progressFinished();
    void progressStarted();
    void progressChanged(qreal progress);

    void tokenChanged();
    void tokenSecretChanged();
    void passwordChanged();

    void revisionChanged( const QString & id, qint64 revision );

private slots:
    bool nextCommand();
    void finished( SmartIODBoxSingle *s );

private:
    SmartIODBoxSingle *getSingle();

private:
    SmartIODBoxPrivate *p;
};

#endif // SMARTIODBOX_H

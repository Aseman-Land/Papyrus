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

#ifndef REPOSITORY_H
#define REPOSITORY_H

#include <QObject>

class RepositoryPrivate;
class Repository : public QObject
{
    Q_OBJECT
public:
    Repository(QObject *parent);
    ~Repository();

    Q_INVOKABLE QString insert(QString path );
    Q_INVOKABLE QString getPath( const QString & id );
    Q_INVOKABLE QString repositoryPath();

    Q_INVOKABLE QString copyImageToRepository(QString path, int size );
    Q_INVOKABLE QString copyAudioToRepository(QString path );

public slots:
    void deleteFile( const QString & f );

private:
    QString generateID(QString path );

private:
    RepositoryPrivate *p;
};

#endif // REPOSITORY_H

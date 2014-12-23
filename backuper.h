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

#ifndef BACKUPER_H
#define BACKUPER_H

#include <QObject>

class Papyrus;
class BackuperPrivate;
class Backuper : public QObject
{
    Q_OBJECT
public:
    Backuper();
    ~Backuper();

public slots:
    void makeBackup(const QString &password = QString());
    bool restore(const QString & path , const QString &password = QString());

signals:
    void success();
    void failed();
    void progress( int percent );

private:
    BackuperPrivate *p;
};

class BackuperCorePrivate;
class BackuperCore : public QObject
{
    Q_OBJECT
public:
    BackuperCore();
    ~BackuperCore();

public slots:
    void makeBackup(const QString & repository_path, const QString & db_path, const QString &cnf_path, const QString &password);
    void restore(const QString & repository_path, const QString & home_path, const QString & path , const QString &password);

signals:
    void success();
    void failed();
    void progress( int percent );

private:
    BackuperCorePrivate *p;
};

#endif // BACKUPER_H

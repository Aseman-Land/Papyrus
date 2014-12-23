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

#ifndef DATABASEDATATYPES_H
#define DATABASEDATATYPES_H

#include <QDateTime>
#include <QColor>
#include <QStringList>
#include <QGeoCoordinate>

class Enums : public QObject
{
    Q_OBJECT
    Q_ENUMS(PaperType)
    Q_ENUMS(SearchPaperType)
public:
    enum PaperType {
        Normal = 0,
        ToDo = 1
    };

    enum SearchPaperType {
        NormalPapers = 1,
        CompletedTasks = 2,
        UncompletedTasks = 4,
        ToDoPapers = 6,
        AllPapers = 7
    };

    Enums(QObject *parent = 0);
    ~Enums();
};

class GroupClass
{
public:
    int id;
    QString name;
    QColor color;
    QString uuid;

    bool operator ==(const GroupClass & g) const {
        return (g.id == id && g.name == name && g.color == color && g.uuid == uuid);
    }
};

class ActivityClass
{
public:
    int id;
    QString name;
    QString description;
    QString image;
};

class PaperClass
{
public:
    PaperClass(){
        id = -1;
        type = Enums::Normal;
        temperature = 0;
    }

    int id;
    QString uuid;
    QString title;
    QString text;
    QDateTime create;
    QDateTime modified;
    QGeoCoordinate location;
    int group;
    int activity;
    QString weather;
    int temperature;
    QStringList files;
    Enums::PaperType type;
};

class SyncItem
{
public:
    SyncItem(){
        revision = -1;
        last_revision = -1;
    }

    QString id;
    int revision;
    int last_revision;
};

typedef QHash<QString,SyncItem> SyncItemHash;

Q_DECLARE_METATYPE( SyncItem )
Q_DECLARE_METATYPE( SyncItemHash )

#endif // DATABASEDATATYPES_H

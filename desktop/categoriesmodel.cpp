/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "categoriesmodel.h"
#include "database.h"

#include <QImage>
#include <QPainterPath>
#include <QPainter>

class CategoriesModelPrivate
{
public:
    Database *db;
    QList<int> groups;
};

CategoriesModel::CategoriesModel(Database *db, QObject *parent) :
    QAbstractListModel(parent)
{
    p = new CategoriesModelPrivate;
    p->db = db;

    connect(p->db, SIGNAL(groupsListChanged()), SLOT(groupsChanged()), Qt::QueuedConnection );
    groupsChanged();
}

int CategoriesModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return id(row);
}

int CategoriesModel::id(int row) const
{
    return p->groups.at(row);
}

int CategoriesModel::indexOf(int gid) const
{
    return p->groups.indexOf(gid);
}

int CategoriesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->groups.count();
}

QVariant CategoriesModel::data(const QModelIndex &index, int role) const
{
    int gid = id(index);
    QVariant res;
    switch( role )
    {
    case Qt::DisplayRole:
        res = p->db->groupName(gid);
        break;

    case Qt::DecorationRole:
    {
        QImage img(16,26,QImage::Format_ARGB32);
        img.fill(QColor(0,0,0,0));
        QPainterPath path;
        path.addEllipse( 8, 10, 6, 6 );
        QPainter painter(&img);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.fillPath(path,p->db->groupColor(gid));
        res = QVariant::fromValue<QImage>(img);
    }
        break;
    }

    return res;
}

void CategoriesModel::groupsChanged()
{
    QList<int> list = p->db->groups();
    for( int i=0; i<list.count(); i++ )
    {
        const int gid = list.at(i);
        if( !p->db->groupName(gid).isEmpty() )
            continue;

        list.removeAt(i);
        i--;
    }

    for( int i=0; i<p->groups.length(); i++ )
    {
        const int gid = p->groups.at(i);
        if( list.contains(gid) )
            continue;

        beginRemoveRows(QModelIndex(),i,i+1);
        p->groups.removeAt(i);
        endRemoveRows();

        i--;
    }

    for( int i=0; i<list.length(); i++ )
    {
        const int gid = list.at(i);
        if( p->groups.contains(gid) )
            continue;

        beginInsertRows(QModelIndex(),i,i+1);
        p->groups.insert(i,gid);
        endInsertRows();
    }

    for( int i=0; i<p->groups.length(); i++ )
    {
        const int gid = p->groups.at(i);
        const int destIdx = list.indexOf(gid);
        if( i == destIdx )
            continue;

        beginMoveRows(QModelIndex(),i,1,QModelIndex(),destIdx);
        p->groups.move(i,destIdx);
        endMoveRows();
        i--;
    }
}

CategoriesModel::~CategoriesModel()
{
    delete p;
}

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

#include "papersmodel.h"
#include "database.h"
#include "simage.h"

#include <QIcon>
#include <QHash>

QHash<QString,SImage> paper_model_colored_imgs;

class PapersModelPrivate
{
public:
    QList<int> papers;
    Database *db;
};

PapersModel::PapersModel(Database *db, QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PapersModelPrivate;
    p->db = db;
}

int PapersModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->papers.count();
}

int PapersModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return id(row);
}

int PapersModel::id(int row) const
{
    return p->papers.at(row);
}

QVariant PapersModel::data(const QModelIndex &index, int role) const
{
    int pid = id(index);
    QVariant res;
    switch( role )
    {
    case Qt::DisplayRole:
        res = p->db->paperTitle(pid) + "<br ><font color=\"#888888\">" + p->db->paperText(pid).replace("\n"," ").left(20) + "...</font>";
        break;

    case Qt::DecorationRole:
    {
        const QColor & clr = p->db->groupColor( p->db->paperGroup(pid) );
        if( paper_model_colored_imgs.contains(clr.name()) )
        {
            res = paper_model_colored_imgs.value(clr.name());
            break;
        }

        SImage image(":/qml/Papyrus/files/paper-item.png");
        image = image.scaled(32,32,Qt::IgnoreAspectRatio,Qt::SmoothTransformation);
        image = image.colorize(clr.rgb());

        paper_model_colored_imgs[clr.name()] = image;
        res = image;
    }
        break;
    }

    return res;
}

void PapersModel::setPapers( const QList<int> & list_const )
{
    QList<int> list = list_const;
    for( int i=0; i<list.count()/2; i++ )
        list.swap( i, list.count()-i-1 );

    for( int i=0; i<p->papers.length(); i++ )
    {
        const int pid = p->papers.at(i);
        if( list.contains(pid) )
            continue;

        beginRemoveRows(QModelIndex(),i,i+1);
        p->papers.removeAt(i);
        endRemoveRows();

        i--;
    }

    for( int i=0; i<list.length(); i++ )
    {
        const int pid = list.at(i);
        if( p->papers.contains(pid) )
            continue;

        beginInsertRows(QModelIndex(),i,i+1);
        p->papers.insert(i,pid);
        endInsertRows();
    }

    for( int i=0; i<p->papers.length(); i++ )
    {
        const int pid = p->papers.at(i);
        const int destIdx = list.indexOf(pid);
        if( i == destIdx )
            continue;

        beginMoveRows(QModelIndex(),i,1,QModelIndex(),destIdx);
        p->papers.move(i,destIdx);
        endMoveRows();
        i--;
    }
}

PapersModel::~PapersModel()
{
    delete p;
}

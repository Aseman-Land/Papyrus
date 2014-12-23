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

#include "datesmodel.h"
#include "database.h"
#include "papyrus.h"
#include "asemantools/asemancalendarconverter.h"

#include <QIcon>

class DatesModelPrivate
{
public:
    Database *db;
    Papyrus *papyrus;
    QList<int> dates;
};

DatesModel::DatesModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new DatesModelPrivate;
    p->papyrus = Papyrus::instance();
    p->db = Papyrus::database();

    connect(p->db, SIGNAL(datesListChanged()), SLOT(datesChanged()), Qt::QueuedConnection );

    p->dates = p->db->dates();
}

int DatesModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return p->dates.at(row);
}

int DatesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->dates.count();
}

QVariant DatesModel::data(const QModelIndex &index, int role) const
{
    int days = id(index);
    QVariant res;
    switch( role )
    {
    case Qt::DisplayRole:
        res = p->papyrus->calendarConverter()->convertIntToStringDate(days);
        break;

    case Qt::DecorationRole:
        res = QVariant::fromValue<QIcon>( QIcon::fromTheme("office-calendar") );
        break;
    }

    return res;
}

void DatesModel::datesChanged()
{
    const QList<int> & list = p->db->dates();

    for( int i=0; i<p->dates.length(); i++ )
    {
        const int days = p->dates.at(i);
        if( list.contains(days) )
            continue;

        beginRemoveRows(QModelIndex(),i,i+1);
        p->dates.removeAt(i);
        endRemoveRows();

        i--;
    }

    for( int i=0; i<list.length(); i++ )
    {
        const int days = list.at(i);
        if( p->dates.contains(days) )
            continue;

        beginInsertRows(QModelIndex(),i,i+1);
        p->dates.insert(i,days);
        endInsertRows();
    }

    for( int i=0; i<p->dates.length(); i++ )
    {
        const int days = p->dates.at(i);
        const int destIdx = list.indexOf(days);
        if( i == destIdx )
            continue;

        beginMoveRows(QModelIndex(),i,1,QModelIndex(),destIdx);
        p->dates.move(i,destIdx);
        endMoveRows();
        i--;
    }
}

DatesModel::~DatesModel()
{
    delete p;
}

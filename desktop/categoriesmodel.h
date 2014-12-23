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

#ifndef CATEGORIESMODEL_H
#define CATEGORIESMODEL_H

#include <QAbstractListModel>

class Database;
class CategoriesModelPrivate;
class CategoriesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    CategoriesModel( Database *db, QObject *parent = 0);
    ~CategoriesModel();

    int id( const QModelIndex &index ) const;
    int id( int row ) const;
    int indexOf( int gid ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

private slots:
    void groupsChanged();

private:
    CategoriesModelPrivate *p;
};

#endif // CATEGORIESMODEL_H

/*
    Copyright (C) 2014 Sialan Labs
    http://labs.sialan.org

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

#ifndef PAPERSMODEL_H
#define PAPERSMODEL_H

#include <QAbstractListModel>

class Database;
class PapersModelPrivate;
class PapersModel : public QAbstractListModel
{
    Q_OBJECT
public:
    PapersModel(Database *db, QObject *parent = 0);
    ~PapersModel();

    int id( const QModelIndex &index ) const;
    int id( int row ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    void setPapers(const QList<int> &papers);

private:
    PapersModelPrivate *p;
};

#endif // PAPERSMODEL_H

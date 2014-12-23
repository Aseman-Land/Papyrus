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

#define ICON_SIZE 64

#include "paperfilesmodel.h"
#include "database.h"
#include "papyrus.h"

#include <QMimeDatabase>
#include <QIcon>
#include <QHash>
#include <QImageReader>
#include <QImage>
#include <QPainter>

class PaperFilesModelPrivate
{
public:
    Database *db;
    int paperId;

    QStringList files;
    QMimeDatabase mime_db;

    QHash<QString,QImage> cached_images;
};

PaperFilesModel::PaperFilesModel(Database *db, QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PaperFilesModelPrivate;
    p->db = db;
    p->paperId = 0;

    connect( p->db, SIGNAL(filesListChanged()), SLOT(filesChanged()) );
}

void PaperFilesModel::setPaper(int pid)
{
    if( p->paperId == pid )
        return;

    p->paperId = pid;
    filesChanged();
}

int PaperFilesModel::paper() const
{
    return p->paperId;
}

QString PaperFilesModel::id(const QModelIndex &index) const
{
    int row = index.row();
    return id(row);
}

QString PaperFilesModel::id(int row) const
{
    return p->files.at(row);
}

QString PaperFilesModel::pathOf(const QModelIndex &idx) const
{
    return Papyrus::instance()->repositoryPath()+"/"+id(idx);
}

int PaperFilesModel::indexOf(const QString & pid) const
{
    return p->files.indexOf(pid);
}

int PaperFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->files.count();
}

QVariant PaperFilesModel::data(const QModelIndex &index, int role) const
{
    const QString & pid = id(index);
    const QString & path = pathOf(index);
    QVariant res;
    switch( role )
    {
    case Qt::DisplayRole:
        res = pid;
        break;

    case Qt::DecorationRole:
    {
        const QMimeType & t = p->mime_db.mimeTypeForFile(path);
        if( t.name().contains("image") )
        {
            QImage img(ICON_SIZE,ICON_SIZE,QImage::Format_ARGB32);
            img.fill(QColor(0,0,0,0));

            if( p->cached_images.contains(path) )
                img = p->cached_images.value(path);
            else
            {
                QImageReader reader(path);
                const QSize & size = reader.size();
                QSize readSize(ICON_SIZE,ICON_SIZE);
                if( size.width()>size.height() )
                    readSize.setHeight( ICON_SIZE*size.height()/size.width() );
                else
                    readSize.setWidth( ICON_SIZE*size.width()/size.height() );

                reader.setScaledSize(readSize);
                const QImage & tmp_img = reader.read();

                QPainter painter(&img);
                painter.fillRect(img.rect(),QColor(0,0,0,0));
                painter.drawImage( QRect(ICON_SIZE/2-readSize.width()/2,ICON_SIZE/2-readSize.height()/2,readSize.width(),readSize.height()),
                                   tmp_img, tmp_img.rect());

                p->cached_images[path] = img;
            }
            res = img;
        }
        else
            res = QIcon::fromTheme(t.iconName());
    }
        break;
    }

    return res;
}

void PaperFilesModel::filesChanged()
{
    QStringList list = (p->paperId? p->db->paperFiles(p->paperId) : QStringList());

    for( int i=0; i<p->files.length(); i++ )
    {
        const QString & pid = p->files.at(i);
        if( list.contains(pid) )
            continue;

        beginRemoveRows(QModelIndex(),i,i+1);
        p->files.removeAt(i);
        endRemoveRows();

        i--;
    }

    for( int i=0; i<list.length(); i++ )
    {
        const QString & pid = list.at(i);
        if( p->files.contains(pid) )
            continue;

        beginInsertRows(QModelIndex(),i,i+1);
        p->files.insert(i,pid);
        endInsertRows();
    }

    for( int i=0; i<p->files.length(); i++ )
    {
        const QString & pid = p->files.at(i);
        const int destIdx = list.indexOf(pid);
        if( i == destIdx )
            continue;

        beginMoveRows(QModelIndex(),i,1,QModelIndex(),destIdx);
        p->files.move(i,destIdx);
        endMoveRows();
        i--;
    }
}

PaperFilesModel::~PaperFilesModel()
{
    delete p;
}

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

#define NORMALIZE_PATH( PATH ) \
    while( PATH.left(7) == "file://" ) \
        PATH = PATH.mid(7);

#include "repository.h"
#include "papyrus.h"

#include <QDir>
#include <QCryptographicHash>
#include <QFileInfo>
#include <QFile>
#include <QDateTime>
#include <QDate>
#include <QImage>
#include <QImageReader>
#include <QImageWriter>
#include <QDebug>

class RepositoryPrivate
{
public:
    Papyrus *papyrus;
};

Repository::Repository(QObject *parent) :
    QObject(parent)
{
    p = new RepositoryPrivate;
    p->papyrus = Papyrus::instance();
}

QString Repository::insert(QString path)
{
    NORMALIZE_PATH(path)
    QString id = generateID(path);
    QString dest = p->papyrus->repositoryPath() + "/" + id;

    if( QFile::exists(dest) )
        return id;

    QFile::copy( path, dest );
    return id;
}

QString Repository::getPath(const QString &id)
{
    return p->papyrus->repositoryPath() + "/" + id;
}

QString Repository::repositoryPath()
{
    return p->papyrus->repositoryPath();
}

QString Repository::copyImageToRepository(QString path, int size)
{
    NORMALIZE_PATH(path)
    const QString & id = generateID( path );
    const QString & dest_path = getPath( id );
    if( QFile::exists(dest_path) )
        return id;

    QImageReader reader(path);

    QSize dest_size = reader.size();
    if( dest_size.width() > dest_size.height() )
    {
        if( dest_size.width() < size )
            size = dest_size.width();

        dest_size.setHeight( size*dest_size.height()/dest_size.width() );
        dest_size.setWidth( size );
    }
    else
    {
        if( dest_size.height() < size )
            size = dest_size.height();

        dest_size.setWidth( size*dest_size.width()/dest_size.height() );
        dest_size.setHeight( size );
    }

    QImage img = reader.read();
    img = img.scaled( dest_size, Qt::IgnoreAspectRatio, Qt::SmoothTransformation );

    QImageWriter writer( dest_path );
    writer.write( img );

    return id;
}

QString Repository::copyAudioToRepository(QString path)
{
    NORMALIZE_PATH(path)
    return insert(path);
}

void Repository::deleteFile(const QString &f)
{
    const QString & path = getPath(f);

    QFile file(path);
    file.remove();

    if( !file.open(QFile::WriteOnly) )
        return;

    file.write("DELETED");
    file.flush();
    file.close();
}

QString Repository::generateID(QString path)
{
    NORMALIZE_PATH(path)
    QFileInfo file(path);
    QString hidden_text = path + " " + QString::number(file.size()) +  " " +
                          file.created().toString("yyyy/MM/dd hh:mm:ss:zzz") + " " +
                          file.lastModified().toString("yyyy/MM/dd hh:mm:ss:zzz");

    QString id = QCryptographicHash::hash(hidden_text.toUtf8(),QCryptographicHash::Md5).toHex() + "." + file.suffix();
    return id;
}

Repository::~Repository()
{
    delete p;
}

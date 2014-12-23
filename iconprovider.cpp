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

#include "iconprovider.h"

#include <QHash>
#include <QIcon>
#include <QPainter>

QHash<QString,QPixmap> provider_pixmap_hash;

IconProvider::IconProvider() :
    QQuickImageProvider( QQuickImageProvider::Pixmap )
{
}

QPixmap IconProvider::requestPixmap( const QString & id, QSize *size, const QSize & requestedSize )
{
    int width  = 22;
    int height = 22;

    if (size)
        *size = QSize(width, height);

    QIcon icon;
    if( id.contains("/") )
        icon = QIcon(id);
    else
        icon = QIcon::fromTheme(id);

    QPixmap pixmap = icon.pixmap( requestedSize.width() > 0 ? requestedSize.width() : width,
                                                  requestedSize.height() > 0 ? requestedSize.height() : height );

    return pixmap;
}

IconProvider::~IconProvider()
{
}

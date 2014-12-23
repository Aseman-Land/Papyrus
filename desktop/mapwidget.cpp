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

#define MAP_PIN_WIDTH 92

#include "mapwidget.h"

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrl>
#include <QSslError>
#include <QPaintEvent>
#include <QPainter>
#include <QDesktopServices>

static QImage *map_widget_pin = 0;

class MapWidgetPrivate
{
public:
    QNetworkAccessManager *manager;
    QNetworkReply *reply;

    qint64 recieved_bytes;
    qint64 total_bytes;

    QImage image;
    bool image_loaded;

    QGeoCoordinate geo;
};

MapWidget::MapWidget(QWidget *parent) :
    QWidget(parent)
{
    p = new MapWidgetPrivate;
    p->reply = 0;
    p->recieved_bytes = -1;
    p->total_bytes = -1;
    p->manager = 0;
    p->image_loaded = false;

    if( !map_widget_pin )
        map_widget_pin = new QImage(QImage(":/qml/Papyrus/files/map-pin.png").scaled(MAP_PIN_WIDTH,MAP_PIN_WIDTH,
                                                                            Qt::IgnoreAspectRatio,
                                                                            Qt::SmoothTransformation) );

    setCursor( Qt::PointingHandCursor );
}

void MapWidget::setGeo(const QGeoCoordinate &geo)
{
    p->image = QImage(":/qml/Papyrus/files/map-loading.png").scaled(width(),height(),
                                                                  Qt::IgnoreAspectRatio,
                                                                  Qt::SmoothTransformation);

    update();
    if( !geo.latitude() && !geo.longitude() )
        return;
    if( p->reply )
        return;

    init_manager();

    p->geo = geo;
    QString path = "http://maps.google.com/maps/api/staticmap?center=" +
            QString::number(geo.latitude()) + "," + QString::number(geo.longitude()) + "&zoom=15&size=" +
            QString::number(width()) + "x" + QString::number(height()) + "&sensor=false";

    QNetworkRequest request = QNetworkRequest(QUrl(path));
    p->reply = p->manager->get(request);

    connect(p->reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
    connect(p->reply, SIGNAL(downloadProgress(qint64,qint64)), SLOT(downloadProgress(qint64,qint64)) );

    setToolTip( QString::number(p->geo.latitude()) + ", " + QString::number(p->geo.longitude()) );
}

void MapWidget::downloadFinished(QNetworkReply *reply)
{
    if( reply != p->reply )
        return;

    p->reply->deleteLater();
    p->reply = 0;
    if (reply->error())
    {
        emit error( QStringList()<<"Failed" );
        emit failed();
        return;
    }

    p->recieved_bytes = -1;
    p->total_bytes = -1;

    const QByteArray & res = reply->readAll();
    p->image = QImage::fromData(res);
    p->image_loaded = true;

    emit finished();
    update();
}

void MapWidget::sslErrors(const QList<QSslError> &list)
{
    QStringList res;
#ifndef Q_OS_IOS
    foreach (const QSslError &error, list)
        res << error.errorString();
#endif
    emit error(res);
}

void MapWidget::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    if( p->recieved_bytes != bytesReceived )
    {
        p->recieved_bytes = bytesReceived;
        emit recievedBytesChanged();
    }
    if( p->total_bytes != bytesTotal )
    {
        p->total_bytes = bytesTotal;
        emit totalBytesChanged();
    }
}

void MapWidget::init_manager()
{
    if( p->manager )
        return;

    p->manager = new QNetworkAccessManager(this);
    connect(p->manager, SIGNAL(finished(QNetworkReply*)), SLOT(downloadFinished(QNetworkReply*)) );
}

void MapWidget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage( rect(), p->image, p->image.rect() );
    if( p->image_loaded )
    {
        painter.drawImage( QRect(width()/2-MAP_PIN_WIDTH/2, height()/2-MAP_PIN_WIDTH,MAP_PIN_WIDTH,MAP_PIN_WIDTH),
                           *map_widget_pin, map_widget_pin->rect() );
    }
    else
    {
        const QString & text = (p->reply? tr("Loading") : tr("No Position"));
        QFontMetrics metric( painter.font() );

        painter.drawText( QRect(0,height()-metric.height(),width(),metric.height()),
                          Qt::AlignCenter | Qt::AlignVCenter, text );
    }
}

void MapWidget::mouseReleaseEvent(QMouseEvent *e)
{
    Q_UNUSED(e)
    QString path = "http://maps.google.com/maps?&q=" + QString::number(p->geo.latitude()) + "," +
                                                       QString::number(p->geo.longitude());
    QDesktopServices::openUrl(path);
}

MapWidget::~MapWidget()
{
    delete p;
}

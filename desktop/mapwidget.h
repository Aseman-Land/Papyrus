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

#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QWidget>
#include <QGeoCoordinate>

class QNetworkReply;
class QSslError;
class MapWidgetPrivate;
class MapWidget : public QWidget
{
    Q_OBJECT
public:
    MapWidget(QWidget *parent = 0);
    ~MapWidget();

public slots:
    void setGeo( const QGeoCoordinate & geo );

signals:
    void recievedBytesChanged();
    void totalBytesChanged();
    void error( const QStringList & error );
    void finished();
    void failed();

private slots:
    void downloadFinished(QNetworkReply *reply);
    void sslErrors(const QList<QSslError> &list);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void init_manager();

protected:
    void paintEvent(QPaintEvent *e);
    void mouseReleaseEvent(QMouseEvent *e);

private:
    MapWidgetPrivate *p;
};

#endif // MAPWIDGET_H

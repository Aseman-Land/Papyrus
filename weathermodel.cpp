/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "weathermodel.h"

#include <qgeopositioninfosource.h>
#include <qgeosatelliteinfosource.h>
#include <qnmeapositioninfosource.h>
#include <qgeopositioninfo.h>
#include <qnetworkconfigmanager.h>
#include <qnetworksession.h>

#include <QSignalMapper>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QStringList>
#include <QTimer>
#include <QUrlQuery>
#include <QElapsedTimer>
#include <QLoggingCategory>
#include <QCoreApplication>
#include <QtMath>

/*
 *This application uses http://openweathermap.org/api
 **/

#define ZERO_KELVIN 273.15

Q_LOGGING_CATEGORY(requestsLog,"wapp.requests")

WeatherData::WeatherData(QObject *parent) :
        QObject(parent)
{
    m_temperature = 0;
}

WeatherData::WeatherData(const WeatherData &other) :
        QObject(0),
        m_dayOfWeek(other.m_dayOfWeek),
        m_weather(other.m_weather),
        m_weatherDescription(other.m_weatherDescription),
        m_temperature(other.m_temperature)
{
}

QString WeatherData::dayOfWeek() const
{
    return m_dayOfWeek;
}

/*!
 * The icon value is based on OpenWeatherMap.org icon set. For details
 * see http://bugs.openweathermap.org/projects/api/wiki/Weather_Condition_Codes
 *
 * e.g. 01d ->sunny day
 *
 * The icon string will be translated to
 * http://openweathermap.org/img/w/01d.png
 */
QString WeatherData::weatherIcon() const
{
    return m_weather;
}

QString WeatherData::weatherDescription() const
{
    return m_weatherDescription;
}

int WeatherData::temperature() const
{
    return m_temperature;
}

void WeatherData::setDayOfWeek(const QString &value)
{
    m_dayOfWeek = value;
    emit dataChanged();
}

void WeatherData::setWeatherIcon(const QString &value)
{
    m_weather = value;
    emit dataChanged();
}

void WeatherData::setWeatherDescription(const QString &value)
{
    m_weatherDescription = value;
    emit dataChanged();
}

void WeatherData::setTemperature(int value)
{
    m_temperature = value;
    emit dataChanged();
}

class WeatherModelPrivate
{
public:
    static const int baseMsBeforeNewRequest = 5 * 1000; // 5 s, increased after each missing answer up to 10x
    QGeoCoordinate coord;
    QString longitude, latitude;
    QNetworkAccessManager *nam;
    WeatherData now;
    QList<WeatherData*> forecast;
    QQmlListProperty<WeatherData> *fcProp;
    QSignalMapper *weatherReplyMapper, *forecastReplyMapper;
    bool ready;
    QElapsedTimer throttle;
    int nErrors;
    int minMsBeforeNewRequest;
    QHash<QString,QString> weather_icons;
    QSet<QNetworkReply*> replies;

    WeatherModelPrivate() :
            nam(NULL),
            fcProp(NULL),
            ready(false),
            nErrors(0),
            minMsBeforeNewRequest(baseMsBeforeNewRequest)
    {
        throttle.invalidate();
    }
};

static void forecastAppend(QQmlListProperty<WeatherData> *prop, WeatherData *val)
{
    Q_UNUSED(val);
    Q_UNUSED(prop);
}

static WeatherData *forecastAt(QQmlListProperty<WeatherData> *prop, int index)
{
    WeatherModelPrivate *d = static_cast<WeatherModelPrivate*>(prop->data);
    return d->forecast.at(index);
}

static int forecastCount(QQmlListProperty<WeatherData> *prop)
{
    WeatherModelPrivate *d = static_cast<WeatherModelPrivate*>(prop->data);
    return d->forecast.size();
}

static void forecastClear(QQmlListProperty<WeatherData> *prop)
{
    static_cast<WeatherModelPrivate*>(prop->data)->forecast.clear();
}

WeatherModel::WeatherModel(QObject *parent) :
        QObject(parent),
        d(new WeatherModelPrivate)
{
    d->weather_icons["01d"] = "weather-clear";
    d->weather_icons["01n"] = "weather-clear";
    d->weather_icons["02d"] = "weather-clear";
    d->weather_icons["02n"] = "weather-clear";
    d->weather_icons["03d"] = "weather-clouds";
    d->weather_icons["03n"] = "weather-clouds";
    d->weather_icons["04d"] = "weather-overcast";
    d->weather_icons["09d"] = "weather-showers";
    d->weather_icons["10d"] = "weather-showers";
    d->weather_icons["11d"] = "weather-storm";
    d->weather_icons["13d"] = "weather-snow";
    d->weather_icons["50d"] = "weather-fog";
    d->weather_icons[""   ] = "weather-unknown";

    d->fcProp = new QQmlListProperty<WeatherData>(this, d,
                                                          forecastAppend,
                                                          forecastCount,
                                                          forecastAt,
                                                          forecastClear);

    d->weatherReplyMapper = new QSignalMapper(this);
    d->forecastReplyMapper = new QSignalMapper(this);

    connect(d->weatherReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleWeatherNetworkData(QObject*)));
    connect(d->forecastReplyMapper, SIGNAL(mapped(QObject*)),
            this, SLOT(handleForecastNetworkData(QObject*)));

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), SLOT(stop()) );
}

WeatherModel::~WeatherModel()
{
    delete d;
}

void WeatherModel::start()
{
    if( d->nam )
        return;

    d->nam = new QNetworkAccessManager(this);
    emit activeChanged();
}

void WeatherModel::stop()
{
//    foreach( QNetworkReply *rply, d->replies )
//    {
//        rply->abort();
//        rply->close();
//        delete rply;
//    }

    d->replies.clear();
    if( d->nam )
    {
        delete d->nam;
        d->nam = 0;
    }

    emit activeChanged();
}

void WeatherModel::hadError(bool tryAgain)
{
    qCDebug(requestsLog) << "hadError, will " << (tryAgain ? "" : "not ") << "rety";
    d->throttle.start();
    if (d->nErrors < 10)
        ++d->nErrors;
    d->minMsBeforeNewRequest = (d->nErrors + 1) * d->baseMsBeforeNewRequest;
}

void WeatherModel::setGeoCoordinate(const QGeoCoordinate &coo)
{
    if( coo == d->coord )
        return;

    d->coord = coo;
    emit geoCoordinateChanged();
    refreshWeather();
}

QGeoCoordinate WeatherModel::geoCoordinate() const
{
    return d->coord;
}

void WeatherModel::refreshWeather()
{
    if( !d->nam )
        return;
    if( !d->coord.isValid() )
        return;

    QString latitude, longitude;
    longitude.setNum( qFloor(d->coord.longitude()*10)/10.0 );
    latitude.setNum( qFloor(d->coord.latitude()*10)/10.0 );

    qCDebug(requestsLog) << "refreshing weather";
    QUrl url("http://api.openweathermap.org/data/2.5/weather");
    QUrlQuery query;

    query.addQueryItem("lat", latitude);
    query.addQueryItem("lon", longitude);
    query.addQueryItem("mode", "json");
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    d->replies.insert(rep);

    // connect up the signal right away
    d->weatherReplyMapper->setMapping(rep, rep);
    connect(rep, SIGNAL(finished()),
            d->weatherReplyMapper, SLOT(map()));
}

void WeatherModel::handleWeatherNetworkData(QObject *replyObj)
{
    if( !d->nam )
        return;

    qCDebug(requestsLog) << "got weather network data";
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        foreach (WeatherData *inf, d->forecast)
            delete inf;
        d->forecast.clear();

        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        if (document.isObject()) {
            QJsonObject obj = document.object();
            QJsonObject tempObject;
            QJsonValue val;

            if (obj.contains(QStringLiteral("weather"))) {
                val = obj.value(QStringLiteral("weather"));
                QJsonArray weatherArray = val.toArray();
                val = weatherArray.at(0);
                tempObject = val.toObject();
                d->now.setWeatherDescription(tempObject.value(QStringLiteral("description")).toString());
                d->now.setWeatherIcon( d->weather_icons.value(tempObject.value("icon").toString()) );
            }
            if (obj.contains(QStringLiteral("main"))) {
                val = obj.value(QStringLiteral("main"));
                tempObject = val.toObject();
                val = tempObject.value(QStringLiteral("temp"));
                d->now.setTemperature(val.toDouble()-ZERO_KELVIN);
            }
        }
    }
    delete networkReply;
    d->replies.remove(networkReply);

    QString latitude, longitude;
    longitude.setNum( qFloor(d->coord.longitude()*10)/10.0 );
    latitude.setNum( qFloor(d->coord.latitude()*10)/10.0 );

    //retrieve the forecast
    QUrl url("http://api.openweathermap.org/data/2.5/forecast/daily");
    QUrlQuery query;

    query.addQueryItem("lat", latitude);
    query.addQueryItem("lon", longitude);
    query.addQueryItem("mode", "json");
    query.addQueryItem("cnt", "5");
    url.setQuery(query);

    QNetworkReply *rep = d->nam->get(QNetworkRequest(url));
    d->replies.insert(rep);

    // connect up the signal right away
    d->forecastReplyMapper->setMapping(rep, rep);
    connect(rep, SIGNAL(finished()), d->forecastReplyMapper, SLOT(map()));
}

void WeatherModel::handleForecastNetworkData(QObject *replyObj)
{
    qCDebug(requestsLog) << "got forecast";
    QNetworkReply *networkReply = qobject_cast<QNetworkReply*>(replyObj);
    if (!networkReply)
        return;

    if (!networkReply->error()) {
        QJsonDocument document = QJsonDocument::fromJson(networkReply->readAll());

        QJsonObject jo;
        QJsonValue jv;
        QJsonObject root = document.object();
        jv = root.value(QStringLiteral("list"));
        if (!jv.isArray())
            qWarning() << "Invalid forecast object";
        QJsonArray ja = jv.toArray();
        //we need 4 days of forecast -> first entry is today
        if (ja.count() != 5)
            qWarning() << "Invalid forecast object";

        int data = 0;
        for (int i = 1; i<ja.count(); i++) {
            WeatherData *forecastEntry = new WeatherData();

            //min/max temperature
            QJsonObject subtree = ja.at(i).toObject();
            jo = subtree.value(QStringLiteral("temp")).toObject();
            jv = jo.value(QStringLiteral("min"));
            data = 0;
            data = jv.toDouble();
            jv = jo.value(QStringLiteral("max"));
            data = (data+jv.toDouble())/2;
            forecastEntry->setTemperature(data-ZERO_KELVIN);

            //get date
            jv = subtree.value(QStringLiteral("dt"));
            QDateTime dt = QDateTime::fromMSecsSinceEpoch((qint64)jv.toDouble()*1000);
            forecastEntry->setDayOfWeek(dt.date().toString(QStringLiteral("ddd")));

            //get icon
            QJsonArray weatherArray = subtree.value(QStringLiteral("weather")).toArray();
            jo = weatherArray.at(0).toObject();
            forecastEntry->setWeatherIcon(jo.value(QStringLiteral("icon")).toString());

            //get description
            forecastEntry->setWeatherDescription(jo.value(QStringLiteral("description")).toString());

            d->forecast.append(forecastEntry);
        }

        if (!(d->ready)) {
            d->ready = true;
            emit readyChanged();
        }

        emit weatherChanged();
    }
    delete networkReply;
    d->replies.remove(networkReply);
}

WeatherData *WeatherModel::weather() const
{
    return &(d->now);
}

QQmlListProperty<WeatherData> WeatherModel::forecast() const
{
    return *(d->fcProp);
}

bool WeatherModel::ready() const
{
    return d->ready;
}

bool WeatherModel::active() const
{
    return d->nam;
}

void WeatherModel::setActive(bool stt)
{
    if( stt )
        start();
    else
        stop();
}

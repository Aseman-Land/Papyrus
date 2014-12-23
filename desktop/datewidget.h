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

#ifndef DATEWIDGET_H
#define DATEWIDGET_H

#include <QWidget>
#include <QDateTime>

class DateWidgetPrivate;
class DateWidget : public QWidget
{
    Q_OBJECT
public:
    DateWidget(QWidget *parent = 0);
    ~DateWidget();

    void setDateVisible( bool stt );
    bool dateVisible() const;

    void setTimeVisible( bool stt );
    bool timeVisible() const;

    void setDateText( const QString & text );
    QString dateText() const;

    void setTimeText( const QString & text );
    QString timeText() const;

    QDateTime dateTime() const;

public slots:
    void setDateTime( const QDateTime & dt );

signals:
    void dateTimeChanged();

private slots:
    void refresh_days();

private:
    DateWidgetPrivate *p;
};

#endif // DATEWIDGET_H

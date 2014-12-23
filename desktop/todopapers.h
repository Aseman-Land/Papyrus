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

#ifndef TODOPAPERS_H
#define TODOPAPERS_H

#include <QWidget>

class ToDoPapersItem;
class ToDoPapersPrivate;
class ToDoPapers : public QWidget
{
    Q_OBJECT
public:
    ToDoPapers(QWidget *parent = 0);
    ~ToDoPapers();

    void setText( const QString & text );
    QString text() const;

signals:
    void textChanged();

private slots:
    void cleanAndRefresh();
    void refreshText();

    void createItemAfter(ToDoPapersItem *item);
    void removeItem(ToDoPapersItem *item);
    void removeNextItem(ToDoPapersItem *item);

    void focusOnUp(ToDoPapersItem *item);
    void focusOnDown(ToDoPapersItem *item);

private:
    void deleteItem(ToDoPapersItem *item);

private:
    ToDoPapersPrivate *p;
};

#endif // TODOPAPERS_H

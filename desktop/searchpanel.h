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

#ifndef SEARCHPANEL_H
#define SEARCHPANEL_H

#include <QWidget>

class SearchPanelPrivate;
class SearchPanel : public QWidget
{
    Q_OBJECT
public:
    SearchPanel(QWidget *parent = 0);
    ~SearchPanel();

signals:
    void founded( const QList<int> & list );

public slots:
    void refreshResult();

private slots:
    void keyword_changed();
    void showDateDomain();
    void showTimeDomain();

protected:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);

private:
    SearchPanelPrivate *p;
};

#endif // SEARCHPANEL_H

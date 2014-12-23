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

#ifndef PANELBOX_H
#define PANELBOX_H

#include <QToolBox>
#include <QModelIndex>

class Papyrus;
class PanelBoxPrivate;
class PanelBox : public QToolBox
{
    Q_OBJECT
public:
    PanelBox(QWidget *parent = 0);
    ~PanelBox();

signals:
    void showPaperRequest( const QList<int> & papers );

private slots:
    void date_selected( const QModelIndex & idx );
    void group_selected( const QModelIndex & idx );
    void showGroupMenu();

private:
    PanelBoxPrivate *p;
};

#endif // PANELBOX_H

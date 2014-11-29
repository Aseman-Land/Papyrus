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

#ifndef GROUPBUTTON_H
#define GROUPBUTTON_H

#include <QComboBox>

class GroupButtonPrivate;
class GroupButton : public QComboBox
{
    Q_OBJECT
public:
    GroupButton(QWidget *parent = 0);
    ~GroupButton();

    int group() const;

public slots:
    void setGroup( int gid );

signals:
    void groupSelected( int gid );

private slots:
    void indexChanged(int row);

protected:
    void paintEvent(QPaintEvent *e);

private:
    GroupButtonPrivate *p;
};

#endif // GROUPBUTTON_H

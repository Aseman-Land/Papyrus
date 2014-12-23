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

#include "groupbutton.h"
#include "categoriesmodel.h"
#include "papyrus.h"
#include "database.h"

#include <QPaintEvent>
#include <QPainter>

class GroupButtonPrivate
{
public:
    CategoriesModel *model;

    QColor color;
    QString text;
};

GroupButton::GroupButton(QWidget *parent) :
    QComboBox(parent)
{
    p = new GroupButtonPrivate;
    p->model = new CategoriesModel(Papyrus::database(),this);

    connect( this, SIGNAL(currentIndexChanged(int)), SLOT(indexChanged(int)) );

    setModel(p->model);
    setCursor(Qt::PointingHandCursor);
    indexChanged(0);
}

void GroupButton::setGroup(int gid)
{
    setCurrentIndex( p->model->indexOf(gid) );
}

int GroupButton::group() const
{
    return p->model->id(currentIndex());
}

void GroupButton::paintEvent(QPaintEvent *e)
{
    QFont fnt = font();
    fnt.setBold(true);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen("#ffffff");
    painter.setFont(fnt);

    painter.fillRect(e->rect(),p->color);
    painter.drawText(rect(), Qt::AlignCenter | Qt::AlignVCenter, p->text);
}

void GroupButton::indexChanged(int row)
{
    Database *db = Papyrus::database();
    const int gid = p->model->id(row);

    p->color = db->groupColor(gid);
    p->text = db->groupName(gid);

    emit groupSelected(gid);
    repaint();
}

GroupButton::~GroupButton()
{
    delete p;
}

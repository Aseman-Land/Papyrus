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

#include "panelbox.h"
#include "categoriesmodel.h"
#include "datesmodel.h"
#include "papyrus.h"
#include "database.h"
#include "searchpanel.h"
#include "addgroupdialog.h"
#include "asemantools/asemancalendarconverter.h"

#include <QListView>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QDebug>

class PanelBoxPrivate
{
public:
    CategoriesModel *groups_model;
    DatesModel *dates_model;

    QListView *groups;
    QListView *dates;
    SearchPanel *search;

    Papyrus *papyrus;
};

PanelBox::PanelBox(QWidget *parent) :
    QToolBox(parent)
{
    p = new PanelBoxPrivate;
    p->papyrus = Papyrus::instance();
    p->groups_model = new CategoriesModel(Papyrus::database(), this);
    p->dates_model = new DatesModel(this);

    p->dates = new QListView(this);
    p->dates->setModel(p->dates_model);
    p->dates->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->dates->setStyleSheet("QListView{ background: #333333; border: 0px solid transparent }"
                            "QListView:focus{ border: 0px solid transparent }"
                            "QListView::item{ color: #ffffff }"
                            "QListView::item:selected{ background: palette(highlight) }");

    p->groups = new QListView(this);
    p->groups->setModel(p->groups_model);
    p->groups->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->groups->setContextMenuPolicy(Qt::CustomContextMenu);
    p->groups->setStyleSheet("QListView{ background: #333333; border: 0px solid transparent }"
                             "QListView:focus{ border: 0px solid transparent }"
                             "QListView::item{ color: #ffffff }"
                             "QListView::item:selected{ background: palette(highlight) }");

#ifdef Q_OS_MAC
    p->dates->setFocusPolicy(Qt::NoFocus);
    p->groups->setFocusPolicy(Qt::NoFocus);
#endif

    p->search = new SearchPanel();

    addItem( p->search, tr("Search") );
    addItem( p->groups, tr("Labels") );
    addItem( p->dates , tr("Dates")  );

    setCurrentIndex(1);
    setStyleSheet("QToolBox{ background: #222222; border: 0px solid transparent; }"
                  "QToolBox::tab { background: #222222; color: #ffffff }"
                  "QToolBox::tab:selected { background: #222222; font: bold; color: white; border-top: 1px solid #333333 }"
                  "QScrollBar:vertical { border: 0px solid transparent; background: transparent; max-width: 5px; min-width: 5px; }"
                  "QScrollBar::handle:vertical { border: 0px solid transparent; background: #666666; width: 5px; min-width: 5px; min-height: 30px }"
                  "QScrollBar::handle:hover { background: palette(highlight); }"
                  "QScrollBar::add-line:vertical { border: 0px solid transparent; background: transparent; height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }"
                  "QScrollBar::sub-line:vertical { border: 0px solid transparent; background: transparent; height: 0px; subcontrol-position: top; subcontrol-origin: margin; }");

    connect( p->dates , SIGNAL(clicked(QModelIndex)), SLOT(date_selected(QModelIndex))  );
    connect( p->groups, SIGNAL(clicked(QModelIndex)), SLOT(group_selected(QModelIndex)) );
    connect( p->search, SIGNAL(founded(QList<int>)) , SIGNAL(showPaperRequest(QList<int>)) );

    connect( p->groups, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showGroupMenu()) );
}

void PanelBox::date_selected(const QModelIndex &idx)
{
    const int days = p->dates_model->id(idx);
    emit showPaperRequest( p->papyrus->database()->papersOf(p->papyrus->calendarConverter()->convertDaysToDate(days)) );
}

void PanelBox::group_selected(const QModelIndex &idx)
{
    const int gid = p->groups_model->id(idx);
    emit showPaperRequest( p->papyrus->database()->papersOf(gid) );
}

void PanelBox::showGroupMenu()
{
    QModelIndex idx = p->groups->indexAt( p->groups->mapFromGlobal(QCursor::pos()) );
    const int row = idx.row();
    if( !idx.isValid() || row == 0 )
        return;

    QMenu menu;
    QAction *edit = menu.addAction( QIcon::fromTheme("edit-rename"), tr("Edit") );
    QAction *dlte = menu.addAction( QIcon::fromTheme("edit-delete"), tr("Delete") );
    QAction *res  = menu.exec( QCursor::pos() );

    if( !res )
        return;

    const int gid = p->groups_model->id(row);
    if( res == edit )
    {
        AddGroupDialog dialog(gid);
        dialog.exec();
    }
    if( res == dlte )
    {
        Database *db = Papyrus::database();
        int del = QMessageBox::warning(this, tr("Delete Label"), tr("Do you realy want to delete \"%1\"?").arg(db->groupName(gid)), QMessageBox::Yes|QMessageBox::No);
        if( del == QMessageBox::Yes )
            db->deleteGroup(gid);
    }
}

PanelBox::~PanelBox()
{
    delete p;
}

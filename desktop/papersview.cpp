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

#include "papersview.h"
#include "papersmodel.h"
#include "database.h"
#include "paperviewdelegate.h"
#include "papyrus.h"
#include "paperconfigure.h"

#include <QScrollBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QPointer>
#include <QHash>

class PapersViewPrivate
{
public:
    Papyrus *papyrus;
    PapersModel *model;
    PaperViewDelegate *delegate;
};

PapersView::PapersView(QWidget *parent) :
    QListView(parent)
{
    p = new PapersViewPrivate;
    p->papyrus = Papyrus::instance();

    p->model = new PapersModel(Papyrus::database(),this);
    p->delegate = new PaperViewDelegate;

    setModel(p->model);
    setItemDelegate(p->delegate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setContextMenuPolicy(Qt::CustomContextMenu);
    setStyleSheet("QListView{ background: #f3f3f3; border: 0px solid transparent }"
                  "QListView::item{ color: #333333; padding: 4px }"
                  "QListView::item:selected{ background: palette(highlight); color: #ffffff }"
                  "QScrollBar:vertical { border: 0px solid transparent; background: transparent; max-width: 5px; min-width: 5px; }"
                  "QScrollBar::handle:vertical { border: 0px solid transparent; background: #aaaaaa; width: 5px; min-width: 5px; min-height: 30px }"
                  "QScrollBar::handle:hover { background: palette(highlight); }"
                  "QScrollBar::add-line:vertical { border: 0px solid transparent; background: transparent; height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }"
                  "QScrollBar::sub-line:vertical { border: 0px solid transparent; background: transparent; height: 0px; subcontrol-position: top; subcontrol-origin: margin; }");

#ifdef Q_OS_MAC
    setFocusPolicy(Qt::NoFocus);
#endif

    connect( this, SIGNAL(clicked(QModelIndex))              , SLOT(paper_selected(QModelIndex)) );
    connect( this, SIGNAL(doubleClicked(QModelIndex))        , SLOT(paper_opened(QModelIndex))   );
    connect( this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(showPaperMenu())             );
}

void PapersView::showPapers(const QList<int> &papers)
{
    p->model->setPapers(papers);
}

void PapersView::paper_selected(const QModelIndex &idx)
{
    const int pid = p->model->id(idx);
    emit paperSelected(pid);
}

void PapersView::paper_opened(const QModelIndex &idx)
{
    const int pid = p->model->id(idx);
    emit paperOpened(pid);
}

void PapersView::showPaperMenu()
{
    QModelIndex idx = indexAt( mapFromGlobal(QCursor::pos()) );
    const int row = idx.row();
    if( !idx.isValid() )
        return;

    QMenu menu;
    QAction *cnf  = menu.addAction( QIcon::fromTheme("document-properties"), tr("Properties") );
    QAction *dlte = menu.addAction( QIcon::fromTheme("edit-delete"), tr("Delete") );
    QAction *res  = menu.exec( QCursor::pos() );

    if( !res )
        return;

    const int pid = p->model->id(row);
    if( res == cnf )
    {
        PaperConfigure::showConfigure(pid);
    }
    if( res == dlte )
    {
        Database *db = Papyrus::database();
        int del = QMessageBox::warning(this, tr("Delete Label"), tr("Do you realy want to delete \"%1\"?").arg(db->paperTitle(pid)), QMessageBox::Yes|QMessageBox::No);
        if( del == QMessageBox::Yes )
            db->deletePaper(pid);
    }
}

PapersView::~PapersView()
{
    delete p;
}

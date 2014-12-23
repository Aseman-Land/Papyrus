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

#include "editorviewmanager.h"
#include "editorview.h"
#include "papyrus.h"
#include "database.h"
#include "paperconfigure.h"

#include <QStackedWidget>
#include <QTabBar>
#include <QVBoxLayout>
#include <QResizeEvent>
#include <QHash>
#include <QMenu>
#include <QAction>
#include <QDebug>

class EditorViewManagerPrivate
{
public:
    QTabBar *tabbar;
    QStackedWidget *stacked;
    QVBoxLayout *layout;

    EditorView *main_editor;

    QList<int> tabs;
};

EditorViewManager::EditorViewManager(QWidget *parent) :
    QWidget(parent)
{
    p = new EditorViewManagerPrivate;

    p->tabbar = new QTabBar(this);
    p->tabbar->setMovable(true);
    p->tabbar->setTabsClosable(true);
    p->tabbar->setUsesScrollButtons(true);
    p->tabbar->setFocusPolicy(Qt::NoFocus);
    p->tabbar->addTab( tr("Main Paper") );
    p->tabbar->setContextMenuPolicy(Qt::CustomContextMenu);
    QWidget *tab_wgt = p->tabbar->tabButton(0, QTabBar::RightSide);
    if( !tab_wgt )
        tab_wgt = p->tabbar->tabButton(0, QTabBar::LeftSide);
    if( tab_wgt )
        tab_wgt->setDisabled(true);

    p->tabs << 0;

    p->main_editor = new EditorView();
    p->main_editor->installEventFilter(this);

    p->stacked = new QStackedWidget();
    p->stacked->addWidget(p->main_editor);

    p->layout = new QVBoxLayout(this);
    p->layout->addWidget(p->stacked);
    p->layout->setContentsMargins(0,0,0,0);
    p->layout->setSpacing(0);

    connect( p->main_editor   , SIGNAL(saved(int))            , this      , SLOT(paperSaved(int))      );
    connect( p->tabbar        , SIGNAL(tabMoved(int,int))     , this      , SLOT(tabMoved(int,int))    );
    connect( p->tabbar        , SIGNAL(currentChanged(int))   , p->stacked, SLOT(setCurrentIndex(int)) );
    connect( p->tabbar        , SIGNAL(tabCloseRequested(int)), this      , SLOT(close(int))           );
    connect( p->tabbar        , SIGNAL(customContextMenuRequested(QPoint)), SLOT(showTabMenu())        );
    connect( Papyrus::database(), SIGNAL(paperChanged(int))     , this      , SLOT(paperChanged(int)) );
}

QTabBar *EditorViewManager::tabBar() const
{
    return p->tabbar;
}

void EditorViewManager::addPaper(int pid)
{
    p->main_editor->setPaper(0);
    if( pid && p->tabs.contains(pid) )
    {
        p->tabbar->setCurrentIndex( p->tabs.indexOf(pid) );
        return;
    }

    EditorView *editor = new EditorView();
    if( pid != 0 )
        editor->setPaper(pid);

    p->stacked->addWidget(editor);
    p->tabbar->addTab( pid? Papyrus::database()->paperTitle(pid) : tr("New Paper") );
    p->tabbar->setCurrentIndex( p->tabbar->count()-1 );

    p->tabs << pid;

    connect( editor, SIGNAL(saved(int)), SLOT(paperSaved(int)) );
}

void EditorViewManager::addToDo(int pid)
{
    addPaper(pid);
    EditorView *editor = static_cast<EditorView*>(p->stacked->widget(p->stacked->count()-1));
    editor->setType(Enums::ToDo);
}

void EditorViewManager::setMainPaper(int pid)
{
    if( pid && p->tabs.contains(pid) )
    {
        p->tabbar->setCurrentIndex( p->tabs.indexOf(pid) );
        return;
    }

    p->main_editor->setPaper(pid);
    p->stacked->setCurrentWidget(p->main_editor);
    p->tabbar->setCurrentIndex( p->stacked->currentIndex() );
}

void EditorViewManager::tabMoved(int from, int to)
{
    p->tabs.move(from,to);

    QWidget *wgt = p->stacked->widget(from);
    p->stacked->removeWidget(wgt);
    p->stacked->insertWidget(to,wgt);
}

void EditorViewManager::close(int row)
{
    p->tabs.removeAt(row);
    p->tabbar->removeTab(row);
    p->stacked->removeWidget( p->stacked->widget(row) );
}

void EditorViewManager::showTabMenu()
{
    const QPoint & pnt = p->tabbar->mapFromGlobal(QCursor::pos());
    int idx = p->tabbar->tabAt(pnt);
    if( idx == -1 )
        return;

    const int pid = static_cast<EditorView*>(p->stacked->widget(idx))->paperId();
    if( !pid )
        return;

    QMenu menu;
    QAction *cnf = menu.addAction( QIcon::fromTheme("document-properties"), tr("Properties") );
    QAction *cls = menu.addAction( QIcon::fromTheme("tab-close"), tr("Close") );
    if( !p->tabs.at(idx) )
        cls->setDisabled(true);

    QAction *res = menu.exec( QCursor::pos() );

    if( res == cnf )
    {
        PaperConfigure::showConfigure(pid);
    }
    if( res == cls )
    {
        close(idx);
    }
}

void EditorViewManager::paperSaved(int pid)
{
    EditorView *view = static_cast<EditorView*>(sender());
    if( !view )
        return;

    int idx = p->stacked->indexOf(view);
    if( idx == -1 )
        return;
    if( p->main_editor == p->stacked->widget(idx) )
        return;

    p->tabs[idx] = pid;
    p->tabbar->setTabText( idx, Papyrus::database()->paperTitle(pid) );
}

void EditorViewManager::paperChanged(int pid)
{
    if( !p->tabs.contains(pid) )
        return;

    int idx = p->tabs.indexOf(pid);
    p->tabbar->setTabText( idx, Papyrus::database()->paperTitle(pid) );
}

void EditorViewManager::resizeEvent(QResizeEvent *e)
{
    p->tabbar->setFixedWidth(e->size().width());
}

EditorViewManager::~EditorViewManager()
{
    delete p;
}

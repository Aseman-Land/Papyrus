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

#include "todopapers.h"
#include "todopapersitem.h"
#include "asemantools/asemantools.h"

#include <QScrollArea>
#include <QHBoxLayout>
#include <QPushButton>
#include <QDebug>

class ToDoPapersPrivate
{
public:
    QWidget *base_wgt;
    QVBoxLayout *base_layout;

    QScrollArea *area;
    QHBoxLayout *layout;

    QString text;

    QList<ToDoPapersItem*> items;

    bool signal_blocker;
};

ToDoPapers::ToDoPapers(QWidget *parent) :
    QWidget(parent)
{
    p = new ToDoPapersPrivate;
    p->signal_blocker = false;

    QPalette palette;
    palette.setColor( QPalette::Window, "#EDEDED" );
    palette.setColor( QPalette::WindowText, "#333333" );

    p->base_wgt = new QWidget();
    p->base_wgt->setPalette(palette);
    p->base_wgt->setAutoFillBackground(true);

    p->base_layout = new QVBoxLayout(p->base_wgt);
    p->base_layout->addStretch();
    p->base_layout->setContentsMargins(0,0,0,0);
    p->base_layout->setSpacing(2);

    p->area = new QScrollArea();
    p->area->setWidget(p->base_wgt);
    p->area->setWidgetResizable(true);

    p->layout = new QHBoxLayout(this);
    p->layout->addWidget(p->area);
    p->layout->setContentsMargins(0,0,0,0);
    p->layout->setSpacing(1);

    setStyleSheet( "QScrollArea{border: 0px solid transparent; background: transparent}" );
}

void ToDoPapers::setText(const QString &text)
{
    if( p->text == text && !text.isEmpty() )
        return;

    p->text = text;
    setLayoutDirection( AsemanTools::directionOf(text) );

    cleanAndRefresh();
    emit textChanged();
}

QString ToDoPapers::text() const
{
    return p->text;
}

void ToDoPapers::cleanAndRefresh()
{
    while( !p->items.isEmpty() )
        deleteItem(p->items.first());

    const QString & text = p->text;

    int lastPos = 0;
    int pos = text.indexOf("\n",lastPos);
    while( pos != text.length() )
    {
        if( pos == -1 )
            pos = text.length();

        const QString & str = text.mid(lastPos,pos-lastPos);
        const QString & slc = str.mid(0,2);
        bool checkable = (slc=="* " || slc=="- ");

        if( !checkable && !p->items.isEmpty() )
        {
            p->signal_blocker = true;
            ToDoPapersItem *item = p->items.last();
            item->setText( item->text() + "\n" + str );
            p->signal_blocker = false;
        }
        else
        {
            ToDoPapersItem *item = new ToDoPapersItem(this);
            item->setCheckable(checkable);
            item->setText( checkable? str.mid(2) : str );
            item->setChecked( slc=="* " );

            connect( item, SIGNAL(textChanged())   , SLOT(refreshText()) );
            connect( item, SIGNAL(checkedChanged()), SLOT(refreshText()) );

            connect( item, SIGNAL(accepted(ToDoPapersItem*))  , SLOT(createItemAfter(ToDoPapersItem*)) );
            connect( item, SIGNAL(removed(ToDoPapersItem*))   , SLOT(removeItem(ToDoPapersItem*))      );
            connect( item, SIGNAL(removeNext(ToDoPapersItem*)), SLOT(removeNextItem(ToDoPapersItem*))  );

            connect( item, SIGNAL(upPressed(ToDoPapersItem*))  , SLOT(focusOnUp(ToDoPapersItem*))   );
            connect( item, SIGNAL(downPressed(ToDoPapersItem*)), SLOT(focusOnDown(ToDoPapersItem*)) );

            p->items << item;
            p->base_layout->insertWidget(p->base_layout->count()-1,item);
            item->show();
        }

        lastPos = pos+1;
        if( pos != text.length() )
            pos = text.indexOf("\n",lastPos);
    }
}

void ToDoPapers::refreshText()
{
    if( p->signal_blocker )
        return;

    QString result = "";
    for( int i=0; i<p->items.count(); i++ )
    {
        ToDoPapersItem *item = p->items.at(i);
        if( item->isCheckable() )
            result += item->isChecked()? "* " : "- ";

        result += item->text();
        if( i < p->items.count()-1 )
            result += "\n";
    }

    p->text = result;
    emit textChanged();
}

void ToDoPapers::createItemAfter(ToDoPapersItem *item)
{
    int index = p->items.indexOf(item);
    if( index == -1 )
        return;

    QString extra_text = item->text().mid(item->cursorPosition());
    item->setText( item->text().mid(0,item->cursorPosition()) );

    ToDoPapersItem *item_new = new ToDoPapersItem(this);
    item_new->setCheckable( true );
    item_new->setChecked( false );

    connect( item_new, SIGNAL(textChanged())   , SLOT(refreshText()) );
    connect( item_new, SIGNAL(checkedChanged()), SLOT(refreshText()) );

    connect( item_new, SIGNAL(accepted(ToDoPapersItem*))  , SLOT(createItemAfter(ToDoPapersItem*)) );
    connect( item_new, SIGNAL(removed(ToDoPapersItem*))   , SLOT(removeItem(ToDoPapersItem*))      );
    connect( item_new, SIGNAL(removeNext(ToDoPapersItem*)), SLOT(removeNextItem(ToDoPapersItem*))  );

    connect( item_new, SIGNAL(upPressed(ToDoPapersItem*))  , SLOT(focusOnUp(ToDoPapersItem*))   );
    connect( item_new, SIGNAL(downPressed(ToDoPapersItem*)), SLOT(focusOnDown(ToDoPapersItem*)) );

    p->items << item_new;
    p->base_layout->insertWidget(p->base_layout->count()-1,item_new);

    p->signal_blocker = true;
    for( int i=p->items.count()-1; i>index+1; i-- )
    {
        ToDoPapersItem *item_a = p->items.at(i);
        ToDoPapersItem *item_b = p->items.at(i-1);

        item_a->setCheckable( item_b->isCheckable() );
        item_a->setText( item_b->text() );
        item_a->setChecked( item_b->isChecked() );
    }

    ToDoPapersItem *app_item = p->items.at(index+1);
    app_item->setCheckable( true );
    app_item->setText( extra_text );
    app_item->setChecked( false );
    app_item->setTextFocus( true );

    p->signal_blocker = false;
    refreshText();
}

void ToDoPapers::removeItem(ToDoPapersItem *item)
{
    int index = p->items.indexOf(item);
    if( index == -1 )
        return;
    if( index == 0 )
        return;

    ToDoPapersItem *before_item = p->items.at(index-1);
    int before_pos = before_item->text().length();

    p->signal_blocker = true;
    before_item->setText( before_item->text() + item->text() );
    before_item->setCursorPosition( before_pos );
    before_item->setTextFocus( true );
    p->signal_blocker = false;

    deleteItem(item);
    refreshText();
}

void ToDoPapers::removeNextItem(ToDoPapersItem *item)
{
    int index = p->items.indexOf(item);
    if( index == -1 )
        return;
    if( index == p->items.count()-1 )
        return;

    removeItem( p->items.at(index+1) );
}

void ToDoPapers::focusOnUp(ToDoPapersItem *item)
{
    int index = p->items.indexOf(item);
    if( index == -1 )
        return;
    if( index == 0 )
        return;

    ToDoPapersItem *fitem = p->items.at(index-1);
    fitem->setTextFocus(true);
    fitem->setCursorPosition( item->cursorPosition() );
}

void ToDoPapers::focusOnDown(ToDoPapersItem *item)
{
    int index = p->items.indexOf(item);
    if( index == -1 )
        return;
    if( index == p->items.count()-1 )
        return;

    ToDoPapersItem *fitem = p->items.at(index+1);
    fitem->setTextFocus(true);
    fitem->setCursorPosition( item->cursorPosition() );
}

void ToDoPapers::deleteItem(ToDoPapersItem *item)
{
    p->base_layout->removeWidget(item);
    p->items.removeOne(item);
    delete item;
}

ToDoPapers::~ToDoPapers()
{
    delete p;
}

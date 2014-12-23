/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Papyrus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Papyrus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define BUFFER_SIZE 3

#include "papermanager.h"
#include "papyrus.h"
#include "database.h"
#include "asemantools/asemancalendarconverter.h"

#include <QQmlComponent>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQuickView>
#include <QDebug>
#include <QTextDocument>
#include <QFontMetricsF>
#include <QList>
#include <QGuiApplication>
#include <QGeoCoordinate>

class PaperManagerPrivate
{
public:
    QList<QQuickItem*> buffer;

    int type;
    int root;
    QString keyword;

    QString advsearch_keyword;
    QDate advsearch_startDate;
    QDate advsearch_endDate;
    QTime advsearch_startTime;
    QTime advsearch_endTime;
    QRectF advsearch_geo;
    QString advsearch_weather;
    int advsearch_group;
    int advsearch_domain;

    QList<int> papers;

    Papyrus *papyrus;
    int current;
};

PaperManager::PaperManager(QQuickItem *parent) :
    QQuickItem(parent)
{
    p = new PaperManagerPrivate;
    p->papyrus = 0;
    p->current = 0;
    p->root = 0;
    p->type = PaperManager::Clean;
}

QQuickItem *PaperManager::createPaper()
{
    QVariant parent = QVariant::fromValue<QQuickItem*>(this);
    QVariant res;

    QQuickView *view = static_cast<QQuickView*>(window());
    QMetaObject::invokeMethod( view->rootObject(), "createPaper", Q_RETURN_ARG(QVariant, res), Q_ARG(QVariant,parent));

    return res.value<QQuickItem*>();
}

Papyrus *PaperManager::getPapyrus()
{
    if( p->papyrus )
        return p->papyrus;

    QVariant res;

    QQuickView *view = static_cast<QQuickView*>(window());
    QMetaObject::invokeMethod( view->rootObject(), "getPapyrus", Q_RETURN_ARG(QVariant, res));

    p->papyrus = res.value<Papyrus*>();
    return p->papyrus;
}

bool actionLessThan( int a, int b)
{
    return Papyrus::database()->paperCreatedDate(a) > Papyrus::database()->paperCreatedDate(b);
}

void PaperManager::setRoot(int id)
{
    if( p->root == id && p->root != 0)
        return;

    p->root = id;

    switch( p->type )
    {
    case Clean:
        p->papers.clear();
        break;

    case All:
        p->papers = Papyrus::database()->papers();
        break;

    case Search:
        p->papers.clear();
        if( p->keyword.isEmpty() )
            break;

        p->papers = Papyrus::database()->search(p->keyword);
        break;

    case AdvanceSearch:
        p->papers.clear();
        p->papers = Papyrus::database()->advanceSearch(p->advsearch_keyword, p->advsearch_startDate,
                                                     p->advsearch_endDate, p->advsearch_startTime,
                                                     p->advsearch_endTime, p->advsearch_group,
                                                     p->advsearch_domain , p->advsearch_geo,
                                                     p->advsearch_weather);
        break;

    case Date:
        p->papers = Papyrus::database()->papersOf( AsemanCalendarConverter::convertDaysToDate(id) );
        break;

    case Group:
        p->papers = Papyrus::database()->papersOf(id);
        qSort( p->papers.begin(), p->papers.end(), actionLessThan );
        break;
    }

    init_buffer();
    for( int i=0; i<BUFFER_SIZE && i<p->papers.length(); i++ )
        p->buffer[i]->setProperty( "paperItem", p->papers[i] );

    p->current = 0;
    emit rootChanged();
    emit currentPaperChanged();
    emit papersChanged();
    emit currentIndexChanged();
}

void PaperManager::init_buffer()
{
    if( p->buffer.length() == BUFFER_SIZE )
        return;

    for( int i=0; i<BUFFER_SIZE; i++ )
    {
        QQuickItem *paper = createPaper();
        paper->setProperty( "visible", (BUFFER_SIZE-i <= 2) );
        paper->setProperty( "paperZ", i );

        connect( paper, SIGNAL(closed(QVariant))              , SLOT(paperClosed(QVariant))          );
        connect( paper, SIGNAL(entered(QVariant))             , SLOT(paperEntered(QVariant))         );
        connect( paper, SIGNAL(deleted(QVariant))             , SLOT(deletePaper(QVariant))          );
        connect( paper, SIGNAL(saved(int))                    , SLOT(paperSaved(int))                );
        connect( paper, SIGNAL(outMove(QVariant))             , SLOT(outMove(QVariant))              );
        connect( paper, SIGNAL(outMoveFinished())             , SLOT(outMoveFinished())              );

        p->buffer.prepend( paper );
    }
}

void PaperManager::reindexBuffer()
{
    int i = p->buffer.length();
    foreach( QQuickItem *paper, p->buffer )
        paper->setProperty( "paperZ", --i );
}

void PaperManager::load_buffers()
{
    const int prevPaper = p->current-1;
    const int nextPaper = p->current+1;
    const int nextBufferIdx = (p->current==0)? 1 : 2;
    const int prevBufferIdx = (p->current==0)? -1 : 0;

    if( 0 <= nextPaper && nextPaper < p->papers.count() )
    {
        const int idx = nextPaper;
        const int paperId = p->papers[idx];

        p->buffer.at(nextBufferIdx)->setProperty( "paperItem", paperId );
    }
    else
    {
        p->buffer.at(nextBufferIdx)->setProperty( "paperItem", -1 );
    }

    if( 0 <= prevPaper && prevPaper < p->papers.count() )
    {
        const int idx = prevPaper;
        const int paperId = p->papers[idx];

        p->buffer.at(prevBufferIdx)->setProperty( "paperItem", paperId );
    }
    else
    if( prevBufferIdx != -1 )
    {
        p->buffer.at(prevBufferIdx)->setProperty( "paperItem", -1 );
    }
}

void PaperManager::paperClosed( const QVariant & var )
{
    QQuickItem *paper = var.value<QQuickItem*>();
    if( p->buffer.indexOf(paper) == 0 && p->current != 0 )
    {
        paper->setProperty( "x" ,paper->property("closeX") );
        return;
    }

    if( p->current >= p->papers.count() )
        QMetaObject::invokeMethod( paper, "save" );

    p->current++;
    emit currentPaperChanged();
    emit currentIndexChanged();

    if( p->current == 1 )
    {
        p->buffer.last()->setProperty( "visible" ,true );
        load_buffers();
        return;
    }

    QQuickItem *top = p->buffer.takeFirst();
    p->buffer << top;
    reindexBuffer();

    top->setProperty( "anim" ,false );
    top->setProperty( "x" ,0 );

    top->setProperty( "opacity", 0 );
    QMetaObject::invokeMethod( top, "startAnimation" );
    top->setProperty( "opacity", 1 );

    load_buffers();
}

void PaperManager::paperEntered( const QVariant & var )
{
    p->current--;
    emit currentPaperChanged();
    emit currentIndexChanged();

    QQuickItem *paper = var.value<QQuickItem*>();
    Q_UNUSED(paper)

    if( p->current == 0 )
        return;

    QQuickItem *btm = p->buffer.takeLast();
    p->buffer.prepend( btm );
    reindexBuffer();

    btm->setProperty( "anim", false );
    btm->setProperty( "x", btm->property("closeX") );
    btm->setProperty( "opacity", 1 );
    btm->setProperty( "visible", true );

    load_buffers();
}

void PaperManager::paperSaved( int id )
{
    if( p->papers.contains(id) )
        return;

    int bufferIdx = (p->current==0)? 0 : 1;
    for( int i=0; i<BUFFER_SIZE; i++ )
        if( p->buffer.at(i)->property("paperItem") == id )
        {
            bufferIdx = i;
            break;
        }

    const int currentIdx = (p->current==0)? p->current + bufferIdx : p->current + bufferIdx -1;

    p->papers.insert( currentIdx, id );
    emit papersChanged();
}

void PaperManager::deletePaper(const QVariant & var)
{
    QQuickItem *paper = var.value<QQuickItem*>();
    if( !paper )
        return;

    const int paperId = paper->property("paperItem").toInt();
    if( paperId == -1 )
        return;

    const int bufferIdx  = p->buffer.indexOf(paper);
    const int currentIdx = (p->current==0)? p->current + bufferIdx : p->current + bufferIdx -1;

    if( currentIdx != p->current )
        return;

    p->papers.removeAll(paperId);
    int nextId = (p->current<p->papers.count())? p->papers.at(p->current) : -1;

    paper->setProperty("paperItem", -1 );
    paper->setProperty("paperItem", nextId );

    Papyrus::database()->deletePaper(paperId);
    reindexBuffer();
    load_buffers();
    emit papersChanged();
}

void PaperManager::outMove(QVariant size)
{
    if( p->current == 0 )
        return;

    QVariant var = size;

    QQuickItem *item = p->buffer.first();
    QMetaObject::invokeMethod( item, "outMoveFnc", Q_ARG(QVariant,var) );
}

void PaperManager::outMoveFinished()
{
    if( p->current == 0 )
        return;

    QQuickItem *item = p->buffer.first();
    QMetaObject::invokeMethod( item, "outMoveFinishedFnc" );
}

int PaperManager::root() const
{
    return p->root;
}

void PaperManager::setType(int t)
{
    if( p->type == t )
        return;

    p->type = t;
    emit typeChanged();
}

int PaperManager::type() const
{
    return p->type;
}

void PaperManager::setKeyword(const QString &kw)
{
    if( p->keyword == kw )
        return;

    p->keyword = kw;
    emit keywordChanged();
}

QString PaperManager::keyword() const
{
    return p->keyword;
}

void PaperManager::setAdvanceSearch(const QString &keyword, const QDate &startDate, const QDate &endDate, const QTime &startTime, const QTime &endTime, int group, int domain, const QRectF &geo, const QString &weather)
{
    p->advsearch_keyword = keyword;
    p->advsearch_startDate = startDate;
    p->advsearch_endDate = endDate;
    p->advsearch_startTime = startTime;
    p->advsearch_endTime = endTime;
    p->advsearch_group = group;
    p->advsearch_domain = domain;
    p->advsearch_geo = geo;
    p->advsearch_weather = weather;
}

QQuickItem *PaperManager::currentPaper() const
{
    return p->buffer.at(p->current==0? 0 : 1);
}

QList<int> PaperManager::papers() const
{
    return p->papers;
}

void PaperManager::setCurrentIndex(int idx)
{
    if( idx >= p->papers.count() )
        return;
    if( p->current == idx )
        return;

    p->current = idx;
    emit currentIndexChanged();

    if( p->current == 0 )
    {
        p->buffer.first()->setProperty("paperItem",p->papers[idx]);
        p->buffer.first()->setProperty("x", 0);
        p->buffer.first()->setProperty( "visible", true );
    }
    else
    {
        p->buffer.first()->setProperty("x", p->buffer.first()->property("closeX"));
        p->buffer.first()->setProperty( "visible", true );
        p->buffer[1]->setProperty("paperItem",p->papers[idx]);
    }

    load_buffers();
}

int PaperManager::currentIndex()
{
    return p->current;
}

PaperManager::~PaperManager()
{
    delete p;
}

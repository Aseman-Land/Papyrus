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

#include "paperconfigure.h"
#include "ui_paperconfigure.h"
#include "papyrus.h"
#include "database.h"
#include "mapwidget.h"
#include "datewidget.h"
#include "asemantools/asemancalendarconverter.h"

#include <QTimer>
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHash>

QHash<int,PaperConfigure*> paper_configure_objs;

class PaperConfigurePrivate
{
public:
    int paperId;

    Ui::PaperConfigure *ui;
    MapWidget *map;

    DateWidget *date_wgt;
    QMenu *date_menu;
    QVBoxLayout *date_layout;
    QPushButton *date_btn;

    Papyrus *papyrus;
    int signal_blocker;
};

PaperConfigure::PaperConfigure(int paperId, QWidget *parent) :
    QDialog(parent)
{
    p = new PaperConfigurePrivate;
    p->papyrus = Papyrus::instance();
    p->paperId = paperId;
    p->signal_blocker = false;

    p->map = new MapWidget();

    p->ui = new Ui::PaperConfigure;
    p->ui->setupUi(this);
    p->ui->map_widget->layout()->addWidget(p->map);

    p->date_wgt = new DateWidget();
    p->date_btn = new QPushButton();
    p->date_btn->setText( tr("Apply") );

    p->date_menu = new QMenu(this);
    p->date_layout = new QVBoxLayout(p->date_menu);
    p->date_layout->addWidget(p->date_wgt);
    p->date_layout->addWidget(p->date_btn);
    p->date_layout->setContentsMargins(4,4,4,4);
    p->date_layout->setSpacing(1);

    setAttribute( Qt::WA_DeleteOnClose, true );

    connect( Papyrus::database(), SIGNAL(paperChanged(int))       , SLOT(paper_changed(int)) );
    connect( p->ui->date_btn  , SIGNAL(clicked())               , SLOT(show_date_menu())   );
    connect( p->date_btn      , SIGNAL(clicked())               , SLOT(apply_date())       );
    connect( p->ui->type_combo, SIGNAL(currentIndexChanged(int)), SLOT(typeChanged(int))   );

    QTimer::singleShot( 200, this, SLOT(refresh()) );
    paper_configure_objs.insert(paperId,this);
}

void PaperConfigure::showConfigure(int paperId)
{
    PaperConfigure *conf = paper_configure_objs.value(paperId);
    if( !conf )
        conf = new PaperConfigure(paperId, static_cast<QWidget*>(Papyrus::instance()->view()) );

    conf->show();
}

void PaperConfigure::refresh()
{
    p->signal_blocker = true;

    Database *db = Papyrus::database();

    p->ui->title_line->setText( db->paperTitle(p->paperId) );
    p->ui->cdate_line->setText( p->papyrus->calendarConverter()->convertDateTimeToString(db->paperCreatedDate(p->paperId)) );
    p->ui->mdate_line->setText( p->papyrus->calendarConverter()->convertDateTimeToString(db->paperModifiedDate(p->paperId)) );

    p->ui->type_combo->setCurrentIndex( db->paperType(p->paperId) );
    p->map->setGeo( db->paperLocation(p->paperId) );
    p->date_wgt->setDateTime( db->paperCreatedDate(p->paperId) );

    setWindowTitle( db->paperTitle(p->paperId) );
    setWindowIcon( QIcon::fromTheme("document-properties") );

    p->signal_blocker = false;
}

void PaperConfigure::paper_changed(int pid)
{
    if( p->paperId != pid )
        return;

    refresh();
}

void PaperConfigure::show_date_menu()
{
    QPoint pnt = p->ui->date_btn->mapToGlobal( QPoint(0, p->ui->date_btn->height()) );
    p->date_menu->move(pnt);
    p->date_menu->show();
}

void PaperConfigure::apply_date()
{
    if( p->signal_blocker )
        return;

    Database *db = Papyrus::database();
    db->setPaperCreatedDate( p->paperId, p->date_wgt->dateTime() );
}

void PaperConfigure::typeChanged(int type)
{
    if( p->signal_blocker )
        return;

    Database *db = Papyrus::database();
    db->setPaperType( p->paperId, type );
}

PaperConfigure::~PaperConfigure()
{
    paper_configure_objs.remove(p->paperId);
    delete p->ui;
    delete p;
}

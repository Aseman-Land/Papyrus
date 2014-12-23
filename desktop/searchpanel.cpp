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

#include "searchpanel.h"
#include "papyrus.h"
#include "database.h"
#include "datewidget.h"
#include "categoriesmodel.h"
#include "asemantools/asemancalendarconverter.h"

#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QLine>
#include <QCheckBox>
#include <QPaintEvent>
#include <QPainter>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTimerEvent>
#include <QMenu>
#include <QDebug>

class SearchPanelPrivate
{
public:
    QVBoxLayout *layout;

    QLabel *keyword_label;
    QLineEdit *keyword_line;

    QLabel *date_label;
    QDateTime date_from;
    QDateTime date_to;
    QPushButton *date_btn;

    QLabel *time_label;
    QDateTime time_from;
    QDateTime time_to;
    QPushButton *time_btn;

    QLabel *group_label;
    QComboBox *group_combo;
    CategoriesModel *group_model;

    QLabel *type_label;
    QComboBox *type_combo;

    int search_timer;
};

SearchPanel::SearchPanel(QWidget *parent) :
    QWidget(parent)
{
    p = new SearchPanelPrivate;
    p->search_timer = 0;

    p->keyword_label = new QLabel();
    p->keyword_label->setText( "<font color=\"#ffffff\">" + tr("Keyword") + "</font>" );

    p->keyword_line = new QLineEdit();
    p->keyword_line->setPlaceholderText(tr("keyword"));

    p->date_label = new QLabel();
    p->date_label->setText( "<font color=\"#ffffff\">" + tr("Date Domain") + "</font>" );

    p->date_btn = new QPushButton();
    p->date_btn->setText( tr("Select Date") );

    p->time_label = new QLabel();
    p->time_label->setText( "<font color=\"#ffffff\">" + tr("Time Domain") + "</font>" );

    p->time_btn = new QPushButton();
    p->time_btn->setText( tr("Select Time") );

    p->group_label = new QLabel();
    p->group_label->setText( "<font color=\"#ffffff\">" + tr("Label") + "</font>" );

    p->group_model = new CategoriesModel(Papyrus::database(),this);
    p->group_combo = new QComboBox();
    p->group_combo->setModel(p->group_model);

    p->type_label = new QLabel();
    p->type_label->setText( "<font color=\"#ffffff\">" + tr("Paper Type") + "</font>" );

    p->type_combo = new QComboBox();
    p->type_combo->addItem( tr("All Papers") );
    p->type_combo->addItem( tr("Normal Papers") );
    p->type_combo->addItem( tr("To-Do Papers") );
    p->type_combo->addItem( tr("Completed tasks") );
    p->type_combo->addItem( tr("Uncompleted tasks") );

    p->layout = new QVBoxLayout(this);
    p->layout->addSpacing(10);
    p->layout->addWidget(p->keyword_label);
    p->layout->addWidget(p->keyword_line);
    p->layout->addSpacing(30);
    p->layout->addWidget(p->date_label);
    p->layout->addWidget(p->date_btn);
    p->layout->addSpacing(10);
    p->layout->addWidget(p->time_label);
    p->layout->addWidget(p->time_btn);
    p->layout->addSpacing(10);
    p->layout->addWidget(p->group_label);
    p->layout->addWidget(p->group_combo);
    p->layout->addSpacing(10);
    p->layout->addWidget(p->type_label);
    p->layout->addWidget(p->type_combo);
    p->layout->addStretch();
    p->layout->setContentsMargins(4,4,4,4);
    p->layout->setSpacing(1);

    setStyleSheet( "QLineEdit{ background: #ffffff; border-radius: 3px; border: 1px solid #888888; color: palette(text); min-height: 24px }"
                   "QLineEdit:hover{ border-color: palette(highlight) }"
                   "QLineEdit:focus{ border-color: palette(highlight) }");

    connect( p->keyword_line, SIGNAL(textChanged(QString)), SLOT(keyword_changed()) );
    connect( p->date_btn    , SIGNAL(clicked())           , SLOT(showDateDomain())  );
    connect( p->time_btn    , SIGNAL(clicked())           , SLOT(showTimeDomain())  );

    connect( p->group_combo, SIGNAL(currentIndexChanged(int)), SLOT(refreshResult()) );
    connect( p->type_combo , SIGNAL(currentIndexChanged(int)), SLOT(refreshResult()) );
}

void SearchPanel::refreshResult()
{
    const QString & kwd = p->keyword_line->text();
    if( kwd.isEmpty() && p->date_from.isNull() && p->date_to.isNull() && p->time_from.isNull() &&
        p->time_to.isNull() && p->group_combo->currentIndex()==0 && p->type_combo->currentIndex()==0 )
    {
        emit founded( QList<int>() );
        return;
    }

    QList<int> result = Papyrus::database()->advanceSearch(kwd, p->date_from.date(), p->date_to.date(),
                                                         p->time_from.time(), p->time_to.time(),
                                                         p->group_combo->currentIndex()? p->group_model->id(p->group_combo->currentIndex()) : -1,
                                                         Enums::AllPapers);
    for( int i=0; i<result.count()/2; i++ )
        result.swap( i, result.count()-i-1 );

    emit founded(result);
}

void SearchPanel::keyword_changed()
{
    if( p->search_timer )
        killTimer(p->search_timer);

    p->search_timer = startTimer(500);
}

void SearchPanel::showDateDomain()
{
    DateWidget *from_date = new DateWidget();
    from_date->setTimeVisible(false);
    from_date->setDateText( tr("From date") );

    DateWidget *to_date = new DateWidget();
    to_date->setTimeVisible(false);
    to_date->setDateText( tr("To date") );

    QPushButton *ok_btn = new QPushButton();
    ok_btn->setText("Ok");
    ok_btn->setCheckable(true);

    QPushButton *reset_btn = new QPushButton();
    reset_btn->setText("Reset");
    reset_btn->setCheckable(true);

    QMenu menu;
    QVBoxLayout *layout = new QVBoxLayout(&menu);
    layout->addWidget(from_date);
    layout->addWidget(to_date);
    layout->addWidget(ok_btn);
    layout->addWidget(reset_btn);
    layout->setContentsMargins(6,6,6,6);
    layout->setSpacing(1);

    connect( ok_btn   , SIGNAL(clicked()), &menu, SLOT(close()) );
    connect( reset_btn, SIGNAL(clicked()), &menu, SLOT(close()) );

    menu.setFixedWidth( p->date_btn->width() );
    menu.exec( p->date_btn->mapToGlobal(QPoint(0,p->date_btn->height())) );

    if( ok_btn->isChecked() )
    {
        p->date_from = from_date->dateTime();
        p->date_to   = to_date->dateTime();
        p->date_btn->setText( tr("From: %1").arg(Papyrus::instance()->calendarConverter()->convertDateTimeToLittleString(p->date_from.date())) + "\n" +
                              tr("To: %1").arg(Papyrus::instance()->calendarConverter()->convertDateTimeToLittleString(p->date_to.date())));
    }
    if( reset_btn->isChecked() )
    {
        p->date_from = QDateTime();
        p->date_to   = QDateTime();
        p->date_btn->setText( tr("Select Date") );
    }

    refreshResult();
}

void SearchPanel::showTimeDomain()
{
    DateWidget *from_time = new DateWidget();
    from_time->setDateVisible(false);
    from_time->setTimeText( tr("From time") );

    DateWidget *to_time = new DateWidget();
    to_time->setDateVisible(false);
    to_time->setTimeText( tr("To time") );

    QPushButton *ok_btn = new QPushButton();
    ok_btn->setText("Ok");
    ok_btn->setCheckable(true);

    QPushButton *reset_btn = new QPushButton();
    reset_btn->setText("Reset");
    reset_btn->setCheckable(true);

    QMenu menu;
    QVBoxLayout *layout = new QVBoxLayout(&menu);
    layout->addWidget(from_time);
    layout->addWidget(to_time);
    layout->addWidget(ok_btn);
    layout->addWidget(reset_btn);
    layout->setContentsMargins(6,6,6,6);
    layout->setSpacing(1);

    connect( ok_btn   , SIGNAL(clicked()), &menu, SLOT(close()) );
    connect( reset_btn, SIGNAL(clicked()), &menu, SLOT(close()) );

    menu.setFixedWidth( p->time_btn->width() );
    menu.exec( p->time_btn->mapToGlobal(QPoint(0,p->time_btn->height())) );

    if( ok_btn->isChecked() )
    {
        p->time_from = from_time->dateTime();
        p->time_to   = to_time->dateTime();
        p->time_btn->setText( tr("From: %1").arg(p->time_from.toString("hh:mm")) + "\n" +
                              tr("To: %1").arg(p->time_to.toString("hh:mm")) );
    }
    if( reset_btn->isChecked() )
    {
        p->time_from = QDateTime();
        p->time_to   = QDateTime();
        p->time_btn->setText( tr("Select Time") );
    }

    refreshResult();
}

void SearchPanel::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    painter.fillRect(e->rect(),"#333333");
}

void SearchPanel::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == p->search_timer )
    {
        killTimer(p->search_timer);
        p->search_timer = 0;
        refreshResult();
    }

    QWidget::timerEvent(e);
}

SearchPanel::~SearchPanel()
{
    delete p;
}

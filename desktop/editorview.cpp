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

#define PAPER_TOP 23
#define PAPER_LFT 22
#define PAPER_RGT 31
#define PAPER_BTM 41
#define PAPER_WDT 600
#define PAPER_HGT 836
#define PAPER_BRD 15
#define PAPER_SNC 4
#define PAPER_CLP 64

#define FILES_HEIGHT 250

#include "editorview.h"
#include "papyrus.h"
#include "database.h"
#include "editorview.h"
#include "groupbutton.h"
#include "asemantools/asemantools.h"
#include "asemantools/asemancalendarconverter.h"
#include "papyrussync.h"
#include "paperfilesview.h"
#include "simage.h"
#include "papertextarea.h"

#include <QTextEdit>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSplitter>
#include <QPainter>
#include <QPaintEvent>
#include <QCoreApplication>
#include <QTimerEvent>
#include <QLabel>
#include <QLinearGradient>
#include <QPushButton>
#include <QDebug>

QImage *back_image = 0;
QImage *papers_image = 0;
QImage *paper_clip = 0;
QImage *paper_clip_off = 0;

class EditorViewPrivate
{
public:
    QVBoxLayout *main_layout;
    QHBoxLayout *top_layout;

    PaperTextArea *body;
    QLineEdit *title;
    GroupButton *group;
    QLabel *date;
    PaperFilesView *files;

    QFont title_font;
    QFont body_font;
    QFont group_font;
    QFont date_font;

    QPushButton *attach_btn;

    int paperId;
    int save_timer;
    bool signal_blocker;
    bool synced;
    bool has_files;

    QImage attach_img;
};

EditorView::EditorView(QWidget *parent) :
    QWidget(parent)
{
    p = new EditorViewPrivate;
    p->save_timer = 0;
    p->paperId = 0;
    p->signal_blocker = false;
    p->synced = true;
    p->has_files = false;

    if( !back_image )
        back_image = new QImage(":/qml/Papyrus/files/background.jpg");
    if( !papers_image )
        papers_image = new QImage(":/qml/Papyrus/files/paper.png");
    if( !paper_clip )
        paper_clip = new QImage(":/qml/Papyrus/files/paper-clip.png");
    if( !paper_clip_off )
        paper_clip_off = new QImage(":/qml/Papyrus/files/paper-clip-off.png");

    p->attach_img = *paper_clip;

    p->title_font = Papyrus::instance()->titleFont();
    p->body_font = Papyrus::instance()->bodyFont();

    p->group_font.setFamily("Droid Kaqaz Sans");
    p->group_font.setPointSize(9);

    p->date_font.setFamily("Droid Kaqaz Sans");
    p->date_font.setPointSize(8);

    p->group = new GroupButton(this);
    p->group->move(25,PAPER_BRD-1);
    p->group->setFixedSize(110,30);
    p->group->setFont(p->group_font);

    p->title = new QLineEdit();
    p->title->setPlaceholderText( tr("Title") );
    p->title->setAlignment(Qt::AlignHCenter);
    p->title->setFont(p->title_font);
    p->title->setStyleSheet("QLineEdit{background: transparent; border: 0px solid translarent}");

    p->body = new PaperTextArea();
    p->body->setPlaceholderText( tr("Text...") );
    p->body->setViewFont(p->body_font);
    p->body->setStyleSheet("QTextEdit{background: transparent; border: 0px solid translarent}");

    p->date = new QLabel(this);
    p->date->setFixedWidth(200);
    p->date->setFont(p->date_font);

    p->top_layout = new QHBoxLayout();
    p->top_layout->addSpacing(p->group->width());
    p->top_layout->addWidget(p->title);
    p->top_layout->addSpacing(p->group->width());
    p->top_layout->setContentsMargins(0,0,0,0);
    p->top_layout->setSpacing(0);

    p->main_layout = new QVBoxLayout(this);
    p->main_layout->addLayout(p->top_layout);
    p->main_layout->addWidget(p->body);
    p->main_layout->setContentsMargins(30,20,30,45);
    p->main_layout->setSpacing(0);

    p->attach_btn = new QPushButton(this);
    p->attach_btn->setFixedSize( PAPER_CLP, PAPER_CLP );
    p->attach_btn->move( width()-PAPER_CLP, height()-PAPER_CLP );
    p->attach_btn->setStyleSheet("QPushButton{ border: 0px solid transparent; background: transparent }");
    p->attach_btn->setCursor(Qt::PointingHandCursor);

    p->files = new PaperFilesView(this);
    p->files->setFixedSize( width(), FILES_HEIGHT );
    p->files->move( 0, height()-FILES_HEIGHT );
    p->files->hide();

    setStyleSheet("QScrollBar:vertical { border: 0px solid transparent; background: transparent; max-width: 5px; min-width: 5px; }"
                  "QScrollBar::handle:vertical { border: 0px solid transparent; background: #aaaaaa; width: 5px; min-width: 5px; min-height: 30px }"
                  "QScrollBar::handle:hover { background: palette(highlight); }"
                  "QScrollBar::add-line:vertical { border: 0px solid transparent; background: transparent; height: 0px; subcontrol-position: bottom; subcontrol-origin: margin; }"
                  "QScrollBar::sub-line:vertical { border: 0px solid transparent; background: transparent; height: 0px; subcontrol-position: top; subcontrol-origin: margin; }" );

    connect( p->title, SIGNAL(textChanged(QString)), SLOT(delayedSave()) );
    connect( p->body , SIGNAL(textChanged())       , SLOT(delayedSave()) );
    connect( p->group, SIGNAL(groupSelected(int))  , SLOT(delayedSave()) );

    connect( p->attach_btn, SIGNAL(clicked()), p->files, SLOT(show()) );

    connect( Papyrus::instance(), SIGNAL(titleFontChanged())          , SLOT(titleFontChanged())           );
    connect( Papyrus::instance(), SIGNAL(bodyFontChanged())           , SLOT(bodyFontChanged())            );
    connect( Papyrus::database(), SIGNAL(revisionChanged(QString,int)), SLOT(revisionChanged(QString,int)) );
    connect( Papyrus::database(), SIGNAL(paperChanged(int))           , SLOT(paperChanged(int))            );
}

int EditorView::paperId() const
{
    return p->paperId;
}

void EditorView::setType(int type)
{
    p->body->setType(type);
}

void EditorView::setPaper(int pid)
{
    save();

    p->signal_blocker = true;

    p->paperId = pid;
    Database *db = Papyrus::database();
    if( !p->paperId )
    {
        p->title->setText(QString());
        p->body->setText(QString());
        p->body->setType(0);
        p->group->setGroup(0);
        p->date->setText(QString());
        p->files->setPaper(0);
        p->signal_blocker = false;
        p->has_files = false;
        update();
        return;
    }

    p->title->setText( db->paperTitle(pid) );
    p->body->setText( db->paperText(pid) );
    p->body->setType( db->paperType(pid) );
    p->group->setGroup( db->paperGroup(pid) );
    p->date->setText( "<font color=\"#888888\">" + Papyrus::instance()->calendarConverter()->convertDateTimeToString(db->paperCreatedDate(pid)) + "</font>" );
    p->files->setPaper(pid);
    p->synced = (db->revisionOf(db->paperUuid(pid))!=-1);
    p->attach_img = SImage(*paper_clip).colorize(db->groupColor(p->group->group()).rgba());
    p->has_files = !db->paperFiles(pid).isEmpty();

    p->signal_blocker = false;
    update();
}

void EditorView::save()
{
    if( p->title->text().isEmpty() && p->body->text().isEmpty() )
        return;

    Database *db = Papyrus::database();
    if( p->paperId == 0 )
        p->paperId = db->createPaper();

    db->setSignalBlocker(true);
    db->setPaper( p->paperId, p->title->text(), p->body->text(), p->group->group() );
    db->setSignalBlocker(false);

    p->attach_img = SImage(*paper_clip).colorize(db->groupColor(p->group->group()).rgba());
    p->files->setPaper(p->paperId);

    emit saved(p->paperId);
    update();
}

void EditorView::delayedSave()
{
    if( p->signal_blocker )
        return;
    if( p->save_timer )
        killTimer(p->save_timer);

    p->save_timer = startTimer(1000);
}

void EditorView::titleFontChanged()
{
    p->title_font = Papyrus::instance()->titleFont();
    p->title->setFont(p->title_font);
}

void EditorView::bodyFontChanged()
{
    p->body_font = Papyrus::instance()->bodyFont();
    p->body->setViewFont(p->body_font);
}

void EditorView::revisionChanged(const QString &iid, int revision)
{
    if( !p->paperId )
        return;

    const QString & uuid = Papyrus::database()->paperUuid(p->paperId);
    if( uuid != iid )
        return;

    p->synced = (revision!=-1);
    update();
}

void EditorView::paperChanged(int id)
{
    if( p->paperId != id )
        return;

    setPaper(p->paperId);
}

void EditorView::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage( rect(), *back_image, back_image->rect() );

    QRect tl_rct_src( 0, 0, PAPER_LFT, PAPER_TOP );
    QRect t_rct_src( PAPER_LFT, 0, PAPER_WDT-PAPER_LFT-PAPER_RGT, PAPER_TOP );
    QRect tr_rct_src( PAPER_WDT-PAPER_RGT, 0, PAPER_RGT, PAPER_TOP );
    QRect r_rct_src( PAPER_WDT-PAPER_RGT, PAPER_TOP, PAPER_RGT, PAPER_HGT-PAPER_TOP-PAPER_BTM );
    QRect br_rct_src( PAPER_WDT-PAPER_RGT, PAPER_HGT-PAPER_BTM, PAPER_RGT, PAPER_BTM );
    QRect b_rct_src( PAPER_LFT, PAPER_HGT-PAPER_BTM, PAPER_WDT-PAPER_LFT-PAPER_RGT, PAPER_BTM );
    QRect bl_rct_src( 0, PAPER_HGT-PAPER_BTM, PAPER_LFT, PAPER_BTM );
    QRect l_rct_src( 0, PAPER_TOP, PAPER_LFT, PAPER_HGT-PAPER_TOP-PAPER_BTM );

    QRect tl_rct_dst( 0, 0, PAPER_BRD, PAPER_BRD );
    QRect t_rct_dst( PAPER_BRD, 0, width()-2*PAPER_BRD, PAPER_BRD );
    QRect tr_rct_dst( width()-PAPER_BRD, 0, PAPER_BRD, PAPER_BRD );
    QRect r_rct_dst( width()-PAPER_BRD, PAPER_BRD, PAPER_BRD, height()-2*PAPER_BRD );
    QRect br_rct_dst( width()-PAPER_BRD, height()-PAPER_BRD, PAPER_BRD, PAPER_BRD );
    QRect b_rct_dst( PAPER_BRD, height()-PAPER_BRD, width()-2*PAPER_BRD, PAPER_BRD );
    QRect bl_rct_dst( 0, height()-PAPER_BRD, PAPER_BRD, PAPER_BRD );
    QRect l_rct_dst( 0, PAPER_BRD, PAPER_BRD, height()-2*PAPER_BRD );

    QRect paper_rect( PAPER_BRD, PAPER_BRD, width()-2*PAPER_BRD, height()-2*PAPER_BRD );
    QRect sync_rect( PAPER_BRD, height()-PAPER_BRD-PAPER_SNC, width()-2*PAPER_BRD, PAPER_SNC );

    painter.drawImage( tl_rct_dst, *papers_image, tl_rct_src );
    painter.drawImage( t_rct_dst , *papers_image, t_rct_src  );
    painter.drawImage( tr_rct_dst, *papers_image, tr_rct_src );
    painter.drawImage( r_rct_dst , *papers_image, r_rct_src  );
    painter.drawImage( br_rct_dst, *papers_image, br_rct_src );
    painter.drawImage( b_rct_dst , *papers_image, b_rct_src  );
    painter.drawImage( bl_rct_dst, *papers_image, bl_rct_src );
    painter.drawImage( l_rct_dst , *papers_image, l_rct_src  );

    painter.fillRect( paper_rect, "#EDEDED" );

    QLinearGradient gradient( QPoint(PAPER_BRD,height()-PAPER_BRD),
                              QPoint(width()-PAPER_BRD,height()-PAPER_BRD) );
    gradient.setColorAt(0.1, QColor(0,0,0,0));
    gradient.setColorAt(0.3, QColor(p->synced?"#50ab99":"#C51313"));
    gradient.setColorAt(0.7, QColor(p->synced?"#50ab99":"#C51313"));
    gradient.setColorAt(0.9, QColor(0,0,0,0));

    if( p->paperId && Papyrus::instance()->papyrusSync()->tokenAvailable() )
        painter.fillRect( sync_rect, gradient );

    QImage & clip_img = !p->has_files? *paper_clip_off : p->attach_img;
    painter.drawImage( QRect(width()-PAPER_CLP,height()-PAPER_CLP,PAPER_CLP,PAPER_CLP),
                       clip_img, clip_img.rect() );
}

void EditorView::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == p->save_timer )
    {
        killTimer(p->save_timer);
        p->save_timer = 0;

        save();
    }

    QWidget::timerEvent(e);
}

void EditorView::resizeEvent(QResizeEvent *e)
{
    Q_UNUSED(e)
    p->date->move(20, height()-PAPER_BRD-p->date->height());
    p->attach_btn->move( width()-PAPER_CLP, height()-PAPER_CLP );
    p->files->setFixedSize( width(), FILES_HEIGHT );
    p->files->move( 0, height()-FILES_HEIGHT );
}

EditorView::~EditorView()
{
    delete p;
}

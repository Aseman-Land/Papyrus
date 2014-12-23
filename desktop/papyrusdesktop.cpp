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

#include "papyrusdesktop.h"
#include "papyrus.h"
#include "database.h"
#include "repository.h"
#include "papyrussync.h"
#include "panelbox.h"
#include "asemantools/asemandevices.h"
#include "asemantools/asemantools.h"
#include "aboutdialog.h"
#include "papersview.h"
#include "editorview.h"
#include "editorviewmanager.h"
#include "configurepage.h"
#include "scolor.h"
#include "addgroupdialog.h"
#include "asemantools/asemandesktoptools.h"
#include "asemantools/asemantools.h"

#include <QToolBar>
#include <QAction>
#include <QListView>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFontDatabase>
#include <QApplication>
#include <QSplitter>
#include <QTimerEvent>
#include <QTabBar>
#include <QPaintEvent>
#include <QPainter>
#include <QProgressBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QStyleFactory>
#include <QMenu>
#include <QDebug>

#ifdef Q_OS_WIN
#include <QtWin>
#endif

#ifdef Q_OS_WIN
#define TOOLBAR_HEIGHT   32
#else
#ifdef Q_OS_MAC
#define TOOLBAR_HEIGHT   34
#else
#define TOOLBAR_HEIGHT   40
#endif
#endif
#define SYNC_PBAR_HEIGHT 8

class PapyrusDesktopPrivate
{
public:
    Papyrus *papyrus;
    AsemanDesktopTools *desktop;

    QWidget *main_widget;
    QVBoxLayout *main_layout;

    QHBoxLayout *toolbar_layout;
    QToolBar *toolbar;
    QAction *new_act;
    QAction *new_todo_act;
    QAction *new_grp_act;
    QAction *conf_act;
    QAction *rsync_act;
    QAction *about_act;

    Database *db;
    Repository *repository;
    PapyrusSync *sync;

    PanelBox *panel;
    PapersView *papers_view;
    EditorViewManager *editor;
    QSplitter *splitter;

    QTabBar *tabbar;
    QVBoxLayout *tabbar_layout;
    QProgressBar *sync_pbar;

    QFont font;

    int splitter_save_timer;
    int resize_save_timer;
};

PapyrusDesktop::PapyrusDesktop() :
    QMainWindow()
{
    p = new PapyrusDesktopPrivate;
    p->splitter_save_timer = 0;
    p->resize_save_timer = 0;
    p->papyrus = Papyrus::instance();

    p->desktop = new AsemanDesktopTools(this);

    QFontDatabase::addApplicationFont( p->papyrus->resourcePathAbs() + "/fonts/DroidKaqazSans.ttf" );

    p->font.setPointSize(10);

    resize( Papyrus::instance()->size() );

#ifdef Q_OS_MAC
    QToolBar *mac_toolbar = new QToolBar();
    mac_toolbar->setFixedHeight(TOOLBAR_HEIGHT);
    mac_toolbar->setMovable(false);

    addToolBar(mac_toolbar);
    setUnifiedTitleAndToolBarOnMac(true);
#endif

    p->main_widget = new QWidget(this);
    p->main_widget->resize(size());

    p->main_layout = new QVBoxLayout(p->main_widget);
    p->main_layout->setContentsMargins(0,0,0,0);
    p->main_layout->setSpacing(0);

    init_toolbar();
    init_mainWidget();

#ifdef Q_OS_WIN
    QtWin::enableBlurBehindWindow(this);
    QtWin::extendFrameIntoClientArea(this,-1,-1,-1,-1);
    setStyleSheet("PapyrusDesktop{border: 0px solid transparent; background: transparent}");
#endif
}

void PapyrusDesktop::init_toolbar()
{
    p->new_act = new QAction( QIcon::fromTheme("document-new"), tr("Add Paper"), this );
    p->new_todo_act = new QAction( QIcon::fromTheme("document-new"), tr("Add To-Do"), this );
    p->new_grp_act = new QAction( QIcon::fromTheme("document-new"), tr("Add Label"), this );
    p->rsync_act = new QAction( QIcon::fromTheme("view-refresh"), tr("Sync"), this );
    p->conf_act = new QAction( QIcon::fromTheme("configure"), tr("Configure"), this );
    p->about_act = new QAction( QIcon::fromTheme("help-about"), tr("About"), this );

    QPalette palette;
    palette.setColor( QPalette::Window, p->desktop->titleBarColor() );
    palette.setColor( QPalette::WindowText, p->desktop->titleBarTextColor() );
    palette.setColor( QPalette::Button, p->desktop->titleBarColor() );
    palette.setColor( QPalette::ButtonText, p->desktop->titleBarTextColor() );

    QAction *new_menu_act = new QAction(QIcon::fromTheme("document-new"), tr("Add Paper"), this);

    new_menu_act->setMenu( new QMenu() );
    new_menu_act->menu()->addAction(p->new_act);
    new_menu_act->menu()->addAction(p->new_todo_act);

    connect( new_menu_act, SIGNAL(triggered()), p->new_act, SLOT(trigger()) );

    p->toolbar = new QToolBar();
    p->toolbar->addAction(new_menu_act);
    p->toolbar->addAction(p->new_grp_act);
    p->toolbar->addSeparator();
    p->toolbar->addAction(p->rsync_act);
    p->toolbar->addSeparator();
    p->toolbar->addAction(p->conf_act);
    p->toolbar->addAction(p->about_act);
    p->toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    p->toolbar->setMovable(false);
    p->toolbar->setPalette(palette);
    p->toolbar->setIconSize(QSize(22,22));
#ifndef Q_OS_LINUX
    p->toolbar->setStyleSheet("QToolBar{border: 0px solid transparent; background: transparent}");
#endif

    p->tabbar_layout = new QVBoxLayout();
    p->tabbar_layout->addStretch();
    p->tabbar_layout->setContentsMargins(0,0,0,0);
    p->tabbar_layout->setSpacing(0);

    QWidget *wgt = new QWidget();
    wgt->setFixedHeight(TOOLBAR_HEIGHT);

    p->toolbar_layout = new QHBoxLayout(wgt);
    p->toolbar_layout->addWidget(p->toolbar);
    p->toolbar_layout->addLayout(p->tabbar_layout);
    p->toolbar_layout->setContentsMargins(0,0,0,0);
    p->toolbar_layout->setSpacing(0);

    p->main_layout->addWidget(wgt);
}

void PapyrusDesktop::init_mainWidget()
{
    p->panel = new PanelBox();
    p->papers_view = new PapersView();
    p->editor = new EditorViewManager();

    SColor titleBarColor = p->desktop->titleBarColor();
    if( p->desktop->titleBarIsDark() )
        titleBarColor = titleBarColor*1.3;

    QPalette palette;
    palette.setColor( QPalette::Window, titleBarColor );
    palette.setColor( QPalette::WindowText, p->desktop->titleBarTextColor() );

    p->tabbar = p->editor->tabBar();
    p->tabbar->setStyle( QStyleFactory::create("Fusion") );
    p->tabbar->setDrawBase(false);
    p->tabbar->setPalette(palette);

    p->tabbar_layout->addWidget(p->tabbar);

    p->splitter = new QSplitter();
    p->splitter->addWidget(p->panel);
    p->splitter->addWidget(p->papers_view);
    p->splitter->addWidget(p->editor);
    p->splitter->setStyleSheet("QSplitter::handle{background: #f3f3f3}");
    p->splitter->setSizes( QList<int>()<< 213 << 273 << width()-486 );
    p->splitter->restoreState( p->papyrus->settings()->value("UserInterface/splitter").toByteArray() );

    p->main_layout->addWidget(p->splitter);

    p->sync_pbar = new QProgressBar(p->main_widget);
    p->sync_pbar->resize( p->editor->width(), SYNC_PBAR_HEIGHT );
    p->sync_pbar->move( width()-p->sync_pbar->width(), height()-SYNC_PBAR_HEIGHT );
    p->sync_pbar->setTextVisible(false);
    p->sync_pbar->setVisible(false);

    connect( p->splitter    , SIGNAL(splitterMoved(int,int))      , this          , SLOT(save_splitter())        );
    connect( p->panel       , SIGNAL(showPaperRequest(QList<int>)), p->papers_view, SLOT(showPapers(QList<int>)) );
    connect( p->papers_view , SIGNAL(paperSelected(int))          , p->editor     , SLOT(setMainPaper(int))      );
    connect( p->papers_view , SIGNAL(paperOpened(int))            , p->editor     , SLOT(addPaper(int))          );
    connect( p->new_act     , SIGNAL(triggered())                 , p->editor     , SLOT(addPaper())             );
    connect( p->new_todo_act, SIGNAL(triggered())                 , p->editor     , SLOT(addToDo())              );
    connect( p->new_grp_act , SIGNAL(triggered())                 , this          , SLOT(addGroup())             );
    connect( p->rsync_act   , SIGNAL(triggered())                 , this          , SLOT(refreshSync())          );
    connect( p->conf_act    , SIGNAL(triggered())                 , this          , SLOT(showConfigure())        );
    connect( p->about_act   , SIGNAL(triggered())                 , this          , SLOT(showAbout())            );
}

void PapyrusDesktop::setDatabase(Database *db)
{
    p->db = db;
}

void PapyrusDesktop::setRepository(Repository *rep)
{
    p->repository = rep;
}

void PapyrusDesktop::setPapyrusSync(PapyrusSync *ksync)
{
    p->sync = ksync;

    connect( p->sync, SIGNAL(syncStarted())      , SLOT(syncStarted())       );
    connect( p->sync, SIGNAL(syncProgress(qreal)), SLOT(syncProgress(qreal)) );
    connect( p->sync, SIGNAL(syncFinished())     , SLOT(syncFinished())      );
}

bool PapyrusDesktop::start()
{
    if( !p->db->password().isEmpty() )
    {
        const QString & pass = QInputDialog::getText(this, tr("Papyrus security"), tr("Please enter password:"), QLineEdit::Password);
        const QString & md5 = AsemanTools::passToMd5(pass);
        if( md5 != p->db->password() )
        {
            QMessageBox::critical(this, tr("Incorrect"), tr("Password is incorrect!"));
            QApplication::exit();
            return false;
        }
    }

    show();
    return true;
}

void PapyrusDesktop::refresh()
{

}

void PapyrusDesktop::lock()
{

}

void PapyrusDesktop::addGroup()
{
    AddGroupDialog dialog(-1);
    dialog.exec();
}

void PapyrusDesktop::showConfigure()
{
    ConfigurePage configure;
    configure.exec();
}

void PapyrusDesktop::showAbout()
{
    AboutDialog about;
    about.exec();
}

void PapyrusDesktop::save_splitter()
{
    if( p->splitter_save_timer )
        killTimer(p->splitter_save_timer);

    p->splitter_save_timer = startTimer(500);
}

void PapyrusDesktop::syncStarted()
{
    p->sync_pbar->setVisible(true);
}

void PapyrusDesktop::syncProgress(qreal val)
{
    p->sync_pbar->setVisible(true);
    p->sync_pbar->setValue( val*100 );
}

void PapyrusDesktop::syncFinished()
{
    p->sync_pbar->setVisible(false);
}

void PapyrusDesktop::refreshSync()
{
    if( !p->sync->tokenAvailable() )
        return;

    p->sync->refreshForce();
    p->sync_pbar->setVisible(true);
    p->sync_pbar->setValue(0);
}

void PapyrusDesktop::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == p->splitter_save_timer )
    {
        killTimer(p->splitter_save_timer);
        p->splitter_save_timer = 0;

        p->papyrus->settings()->setValue("UserInterface/splitter", p->splitter->saveState() );
    }
    else
    if( e->timerId() == p->resize_save_timer )
    {
        killTimer(p->resize_save_timer);
        p->resize_save_timer = 0;

        Papyrus::instance()->setSize(size());
    }

    QWidget::timerEvent(e);
}

void PapyrusDesktop::resizeEvent(QResizeEvent *e)
{
    if( p->resize_save_timer )
        killTimer(p->resize_save_timer);

    p->resize_save_timer = startTimer(500);

    p->main_widget->resize(size());
    p->sync_pbar->resize( p->editor->width(), SYNC_PBAR_HEIGHT );
    p->sync_pbar->move( width()-p->sync_pbar->width(), height()-SYNC_PBAR_HEIGHT );

    QWidget::resizeEvent(e);
}

void PapyrusDesktop::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e)
    QPainter painter(this);
#ifndef Q_OS_WIN
    painter.fillRect(0,0,width(),TOOLBAR_HEIGHT,p->desktop->titleBarColor());
#endif
    painter.fillRect(0,TOOLBAR_HEIGHT,width(),height()-TOOLBAR_HEIGHT,palette().window());
}

PapyrusDesktop::~PapyrusDesktop()
{
    delete p;
}

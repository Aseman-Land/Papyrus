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

#include "addgroupdialog.h"
#include "papyrus.h"
#include "database.h"

#include <QColorDialog>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QDebug>

bool colors_registered = false;
bool registerColors()
{
    if( colors_registered )
        return true;

    int cnt = -1;
    QColorDialog::setCustomColor( ++cnt, "darkgray" );
    QColorDialog::setCustomColor( ++cnt, "dodgerblue" );
    QColorDialog::setCustomColor( ++cnt, "darkgreen" );
    QColorDialog::setCustomColor( ++cnt, "gold" );

    QColorDialog::setCustomColor( ++cnt, "maroon" );
    QColorDialog::setCustomColor( ++cnt, "purple" );
    QColorDialog::setCustomColor( ++cnt, "orangered" );
    QColorDialog::setCustomColor( ++cnt, "magenta" );

    QColorDialog::setCustomColor( ++cnt, "darkslateblue" );
    QColorDialog::setCustomColor( ++cnt, "violet" );
    QColorDialog::setCustomColor( ++cnt, "saddlebrown" );
    QColorDialog::setCustomColor( ++cnt, "black" );

    QColorDialog::setCustomColor( ++cnt, "chocolate" );
    QColorDialog::setCustomColor( ++cnt, "firebrick" );
    QColorDialog::setCustomColor( ++cnt, "teal" );
    QColorDialog::setCustomColor( ++cnt, "darkviolet" );

    QColorDialog::setCustomColor( ++cnt, "olive" );
    QColorDialog::setCustomColor( ++cnt, "mediumvioletred" );
    QColorDialog::setCustomColor( ++cnt, "darkorange" );
    QColorDialog::setCustomColor( ++cnt, "darkslategray" );

    return true;
}

class AddGroupDialogPrivate
{
public:
    QColorDialog *color;

    QLineEdit *name_line;

    QVBoxLayout *layout;
    QHBoxLayout *name_layout;

    int groupId;
};

AddGroupDialog::AddGroupDialog(int gid, QWidget *parent) :
    QDialog(parent)
{
    p = new AddGroupDialogPrivate;
    p->groupId = gid;

    colors_registered = registerColors();

    p->color = new QColorDialog();
    p->color->setWindowFlags(Qt::Widget);
    p->color->setOption( QColorDialog::DontUseNativeDialog, true );

    p->name_line = new QLineEdit();
    p->name_line->setPlaceholderText( tr("Label Name") );

    p->name_layout = new QHBoxLayout();
    p->name_layout->addWidget(p->name_line);
    p->name_layout->setContentsMargins(4,4,4,4);
    p->name_layout->setSpacing(1);

    p->layout = new QVBoxLayout(this);
    p->layout->addLayout(p->name_layout);
    p->layout->addWidget(p->color);
    p->layout->setContentsMargins(0,0,0,0);
    p->layout->setSpacing(1);

    connect( p->color, SIGNAL(accepted()), SLOT(accept()) );
    connect( p->color, SIGNAL(rejected()), SLOT(close())  );

    setWindowTitle(tr("Add Label"));
#ifdef Q_OS_MAC
    setFixedSize(687,478);
#else
    setFixedSize(607,438);
#endif

    if( p->groupId != -1 )
    {
        Database *db = Papyrus::database();
        p->name_line->setText( db->groupName(p->groupId) );
        p->color->setCurrentColor( db->groupColor(p->groupId) );
    }
}

void AddGroupDialog::accept()
{
    Database *db = Papyrus::database();
    if( p->name_line->text().trimmed().isEmpty() )
    {
        p->color->show();
        return;
    }
    if( p->groupId == -1 )
        p->groupId = db->createGroup();

    db->setGroupName( p->groupId, p->name_line->text().trimmed() );
    db->setGroupColor( p->groupId, p->color->selectedColor() );

    QDialog::accept();
}

AddGroupDialog::~AddGroupDialog()
{
    delete p;
}

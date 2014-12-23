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

#include "aboutdialog.h"
#include "ui_aboutdialog.h"
#include "papyrus.h"
#include "asemantools/asemantools.h"

#include <QDesktopServices>
#include <QUrl>

class AboutDialogPrivate
{
public:
    Ui::AboutDialog *ui;
};

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent)
{
    p = new AboutDialogPrivate;
    p->ui = new Ui::AboutDialog;
    p->ui->setupUi(this);
    p->ui->name_label->setText( p->ui->name_label->text() + " " + Papyrus::version() );
    p->ui->qtversion_lbl->setText( p->ui->qtversion_lbl->text() + " Qt" + AsemanTools::qtVersion() );
}

void AboutDialog::donateUs()
{
    QDesktopServices::openUrl( QUrl("http://aseman.co/donate") );
}

AboutDialog::~AboutDialog()
{
    delete p->ui;
    delete p;
}

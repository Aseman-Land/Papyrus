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

#include "uiselectordialog.h"
#include "ui_uiselectordialog.h"

class UiSelectorDialogPrivate
{
public:
    Ui::UiSelectorDialog *ui;
    bool accepted;
};

UiSelectorDialog::UiSelectorDialog(QWidget *parent) :
    QDialog(parent)
{
    p = new UiSelectorDialogPrivate;
    p->accepted = false;

    p->ui = new Ui::UiSelectorDialog;
    p->ui->setupUi(this);
}

bool UiSelectorDialog::isDesktopTouch() const
{
    return p->ui->touch_radio->isChecked();
}

bool UiSelectorDialog::isDesktopClassic() const
{
    return p->ui->classic_radio->isChecked();
}

bool UiSelectorDialog::isAccepted() const
{
    return p->accepted;
}

void UiSelectorDialog::accept()
{
    p->accepted = true;
    QDialog::accept();
}

UiSelectorDialog::~UiSelectorDialog()
{
    delete p->ui;
    delete p;
}

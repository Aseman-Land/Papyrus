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

#ifndef ADDGROUPDIALOG_H
#define ADDGROUPDIALOG_H

#include <QDialog>

class AddGroupDialogPrivate;
class AddGroupDialog : public QDialog
{
    Q_OBJECT
public:
    AddGroupDialog(int gid = -1, QWidget *parent = 0);
    ~AddGroupDialog();

public slots:
    void accept();

private:
    AddGroupDialogPrivate *p;
};

#endif // ADDGROUPDIALOG_H

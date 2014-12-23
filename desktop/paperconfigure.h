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

#ifndef PAPERCONFIGURE_H
#define PAPERCONFIGURE_H

#include <QDialog>

class PaperConfigurePrivate;
class PaperConfigure : public QDialog
{
    Q_OBJECT
public:
    PaperConfigure(int paperId, QWidget *parent = 0);
    ~PaperConfigure();

    static void showConfigure( int paperId );

public slots:
    void refresh();

private slots:
    void paper_changed( int pid );
    void show_date_menu();
    void apply_date();
    void typeChanged( int type );

private:
    PaperConfigurePrivate *p;
};

#endif // PAPERCONFIGURE_H

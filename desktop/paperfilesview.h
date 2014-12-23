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

#ifndef PAPERFILESVIEW_H
#define PAPERFILESVIEW_H

#include <QWidget>

class PaperFilesViewPrivate;
class PaperFilesView : public QWidget
{
    Q_OBJECT
public:
    PaperFilesView(QWidget *parent = 0);
    ~PaperFilesView();

    void setPaper( int pid );
    int paper() const;

private slots:
    void fileClicked( const QModelIndex & idx );
    void fileDoubleClicked( const QModelIndex & idx );
    void showFileMenu();
    void addFiles();

protected:
    void paintEvent(QPaintEvent *e);

private:
    PaperFilesViewPrivate *p;
};

#endif // PAPERFILESVIEW_H

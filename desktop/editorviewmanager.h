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

#ifndef EDITORVIEWMANAGER_H
#define EDITORVIEWMANAGER_H

#include <QWidget>

class QTabBar;
class EditorViewManagerPrivate;
class EditorViewManager : public QWidget
{
    Q_OBJECT
public:
    EditorViewManager(QWidget *parent = 0);
    ~EditorViewManager();

    QTabBar *tabBar() const;

public slots:
    void addPaper( int pid = 0 );
    void addToDo( int pid = 0 );
    void setMainPaper( int pid );

private slots:
    void tabMoved( int from, int to );
    void close( int row );
    void showTabMenu();
    void paperSaved( int pid );
    void paperChanged( int pid );

protected:
    void resizeEvent(QResizeEvent *e);

private:
    EditorViewManagerPrivate *p;
};

#endif // EDITORVIEWMANAGER_H

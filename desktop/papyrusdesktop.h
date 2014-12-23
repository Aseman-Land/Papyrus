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

#ifndef PAPYRUSDESKTOP_H
#define PAPYRUSDESKTOP_H

#include <QMainWindow>

class MimeApps;
class AsemanTools;
class AsemanDevices;
class PapyrusSync;
class Backuper;
class Repository;
class Database;
class Papyrus;
class PapyrusDesktopPrivate;
class PapyrusDesktop : public QMainWindow
{
    Q_OBJECT
public:
    PapyrusDesktop();
    ~PapyrusDesktop();

    void setDatabase( Database *db );
    void setRepository( Repository *rep );
    void setPapyrusSync( PapyrusSync *ksync );

public slots:
    bool start();
    void refresh();
    void lock();

    void addGroup();
    void showConfigure();
    void showAbout();

private slots:
    void save_splitter();
    void syncStarted();
    void syncProgress( qreal val );
    void syncFinished();
    void refreshSync();

protected:
    void timerEvent(QTimerEvent *e);
    void resizeEvent(QResizeEvent *e);
    void paintEvent(QPaintEvent *e);

private:
    void init_toolbar();
    void init_mainWidget();

private:
    PapyrusDesktopPrivate *p;
};

#endif // PAPYRUSDESKTOP_H

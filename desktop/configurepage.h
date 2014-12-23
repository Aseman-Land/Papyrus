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

#ifndef CONFIGUREPAGE_H
#define CONFIGUREPAGE_H

#include <QDialog>

class ConfigurePagePrivate;
class ConfigurePage : public QDialog
{
    Q_OBJECT
public:
    ConfigurePage(QWidget *parent = 0);
    ~ConfigurePage();

public slots:
    void refresh();

private slots:
    void uiChanged(int row );
    void calendarChanged( int id );
    void languageChanged( int id );
    void titleFontChanged();
    void bodyFontChanged();

    void makeBackup();
    void restoreSelected();
    void deleteBackup();
    void copyBackup();

    void backupProgress( int percent );
    void backupFailed();
    void backupFinished();

    void getToken();
    void syncAuthorizeRequest();
    void syncLoginFinished();
    void syncDisconnect();

    void changePassword();

protected:
    void closeEvent(QCloseEvent *e);

private:
    ConfigurePagePrivate *p;
};

#endif // CONFIGUREPAGE_H

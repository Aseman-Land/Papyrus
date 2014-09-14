/*
    Copyright (C) 2014 Sialan Labs
    http://labs.sialan.org

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

#ifndef UISELECTORDIALOG_H
#define UISELECTORDIALOG_H

#include <QDialog>

class UiSelectorDialogPrivate;
class UiSelectorDialog : public QDialog
{
    Q_OBJECT
public:
    UiSelectorDialog(QWidget *parent = 0);
    ~UiSelectorDialog();

    bool isDesktopTouch() const;
    bool isDesktopClassic() const;
    bool isAccepted() const;

public slots:
    void accept();

private:
    UiSelectorDialogPrivate *p;
};

#endif // UISELECTORDIALOG_H

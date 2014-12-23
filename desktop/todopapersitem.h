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

#ifndef TODOPAPERSITEM_H
#define TODOPAPERSITEM_H

#include <QWidget>

class ToDoPapersItemPrivate;
class ToDoPapersItem : public QWidget
{
    Q_OBJECT
public:
    ToDoPapersItem(QWidget *parent = 0);
    ~ToDoPapersItem();

    QString text() const;
    void setText( const QString & txt );

    void setViewFont( const QFont & font );
    QFont viewFont() const;

    void setCheckable( bool stt );
    bool isCheckable() const;

    void setChecked( bool stt );
    bool isChecked() const;

    void setTextFocus( bool stt );
    bool textFocus() const;

    int cursorPosition() const;
    void setCursorPosition( int pos );

signals:
    void textChanged();
    void viewFontChanged();
    void checkableChanged();
    void checkedChanged();
    void textFocusChanged();

    void accepted(ToDoPapersItem *item);
    void removed(ToDoPapersItem *item);
    void removeNext(ToDoPapersItem *item);

    void upPressed(ToDoPapersItem *item);
    void downPressed(ToDoPapersItem *item);

private slots:
    void refreshSize();

protected:
    bool eventFilter(QObject *o, QEvent *e);

private:
    ToDoPapersItemPrivate *p;
};

#endif // TODOPAPERSITEM_H

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

#ifndef PAPERTEXTAREA_H
#define PAPERTEXTAREA_H

#include <QWidget>

class PaperTextAreaPrivate;
class PaperTextArea : public QWidget
{
    Q_OBJECT
public:
    PaperTextArea(QWidget *parent = 0);
    ~PaperTextArea();

    void setType( int t );
    int type() const;

    void setText( const QString & text );
    QString text() const;

    void setPlaceholderText( const QString & txt );
    QString placeholderText() const;

    void setViewFont( const QFont & font );
    QFont viewFont() const;

signals:
    void textChanged();
    void typeChanged();
    void viewFontChanged();

private slots:
    void text_changed();

private:
    PaperTextAreaPrivate *p;
};

#endif // PAPERTEXTAREA_H

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

#ifndef EDITORVIEW_H
#define EDITORVIEW_H

#include <QWidget>

class EditorViewPrivate;
class EditorView : public QWidget
{
    Q_OBJECT
public:
    EditorView(QWidget *parent = 0);
    ~EditorView();

    int paperId() const;

    void setType( int type );

public slots:
    void setPaper( int id );
    void save();

signals:
    void saved( int pid );

private slots:
    void delayedSave();
    void titleFontChanged();
    void bodyFontChanged();
    void revisionChanged( const QString &iid, int revision );
    void paperChanged( int id );

protected:
    void paintEvent(QPaintEvent *e);
    void timerEvent(QTimerEvent *e);
    void resizeEvent(QResizeEvent *e);

private:
    EditorViewPrivate *p;
};

#endif // EDITORVIEW_H

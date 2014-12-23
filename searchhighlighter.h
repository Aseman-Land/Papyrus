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

#ifndef SEARCHHIGHLIGHTER_H
#define SEARCHHIGHLIGHTER_H

#include <QObject>
#include <QQuickTextDocument>

class SearchHighlighterPrivate;
class SearchHighlighter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQuickTextDocument* textDocument READ textDocument WRITE setTextDocument NOTIFY textDocumentChanged)
    Q_PROPERTY(QString searchKeyword READ searchKeyword WRITE setSearchKeyword NOTIFY searchKeywordChanged)
public:
    SearchHighlighter(QObject *parent = 0);
    ~SearchHighlighter();

    void setTextDocument( QQuickTextDocument *doc );
    QQuickTextDocument *textDocument() const;

    void setSearchKeyword( const QString & keyword );
    QString searchKeyword() const;

signals:
    void textDocumentChanged();
    void searchKeywordChanged();

private:
    SearchHighlighterPrivate *p;
};

#endif // SEARCHHIGHLIGHTER_H

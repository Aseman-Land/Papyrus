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

#include "searchhighlighter.h"

#include <QTextCharFormat>
#include <QSyntaxHighlighter>
#include <QTextDocument>
#include <QRegExp>
#include <QPointer>

class SearchHighlighterCore : public QSyntaxHighlighter
{
public:
    SearchHighlighterCore(QTextDocument *parent): QSyntaxHighlighter(parent){}

    QString keyword;

protected:
    void highlightBlock(const QString & text)
    {
        QTextCharFormat myClassFormat;
        myClassFormat.setFontWeight(QFont::Bold);
        myClassFormat.setForeground(Qt::darkBlue);
//        myClassFormat.setFontUnderline(true);

        const QStringList & words = keyword.split(" ",QString::SkipEmptyParts);
        foreach( const QString & word, words )
        {
            int index = text.indexOf(word, 0, Qt::CaseInsensitive);
            while (index >= 0) {
                int length = word.length();
                setFormat(index, length, myClassFormat);
                index = text.indexOf(word, index + length, Qt::CaseInsensitive);
            }
        }
    }

};

class SearchHighlighterPrivate
{
public:
    QString keyword;
    QPointer<SearchHighlighterCore> core;
    QQuickTextDocument *doc;
};

SearchHighlighter::SearchHighlighter(QObject *parent) :
    QObject(parent)
{
    p = new SearchHighlighterPrivate;
}

void SearchHighlighter::setTextDocument(QQuickTextDocument *doc)
{
    if( p->doc == doc )
        return;

    p->doc = doc;
    if( p->core )
        p->core->deleteLater();

    p->core = new SearchHighlighterCore(p->doc->textDocument());
    p->core->keyword = p->keyword;

    emit textDocumentChanged();
}

QQuickTextDocument *SearchHighlighter::textDocument() const
{
    return p->doc;
}

void SearchHighlighter::setSearchKeyword(const QString &keyword)
{
    if( p->keyword == keyword )
        return;

    p->keyword = keyword;
    if( p->core )
    {
        p->core->keyword = p->keyword;
        p->core->rehighlight();
    }

    emit searchKeywordChanged();
}

QString SearchHighlighter::searchKeyword() const
{
    return p->keyword;
}

SearchHighlighter::~SearchHighlighter()
{
    delete p;
}

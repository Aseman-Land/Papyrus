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

#include "papertextarea.h"
#include "database.h"
#include "papyrus.h"
#include "todopapers.h"

#include <QTextEdit>
#include <QHBoxLayout>
#include <QPointer>
#include <QDebug>

class PaperTextAreaPrivate
{
public:
    QString txt;
    QString placeholder;

    int type;
    QFont font;

    QPointer<QTextEdit> text;
    QPointer<ToDoPapers> todo;

    QHBoxLayout *layout;
};

PaperTextArea::PaperTextArea(QWidget *parent) :
    QWidget(parent)
{
    p = new PaperTextAreaPrivate;
    p->type = Enums::Normal;

    p->text = new QTextEdit();

    p->layout = new QHBoxLayout(this);
    p->layout->addWidget(p->text);
    p->layout->setContentsMargins(0,0,0,0);
    p->layout->setSpacing(0);

    connect( p->text, SIGNAL(textChanged()), SLOT(text_changed()) );
}

void PaperTextArea::setType(int t)
{
    if( p->type == t )
        return;

    p->type = t;
    switch( p->type )
    {
    case Enums::Normal:
        if( p->todo )
        {
            p->layout->removeWidget(p->todo);
            delete p->todo.data();
        }

        p->text = new QTextEdit();
        p->text->setText(p->txt);
        p->text->setPlaceholderText(p->placeholder);
        p->text->setFont(p->font);

        p->layout->addWidget(p->text);
        connect( p->text, SIGNAL(textChanged()), SLOT(text_changed()) );
        break;

    case Enums::ToDo:
        if( p->text )
        {
            p->layout->removeWidget(p->text);
            delete p->text.data();
        }

        p->todo = new ToDoPapers();
        p->todo->setText(p->txt);
        p->todo->setFont(p->font);

        p->layout->addWidget(p->todo);
        connect( p->todo, SIGNAL(textChanged()), SLOT(text_changed()) );
        break;
    }

    emit typeChanged();
}

int PaperTextArea::type() const
{
    return p->type;
}

void PaperTextArea::setText(const QString &text)
{
    if( p->txt == text )
        return;

    p->txt = text;
    switch( p->type )
    {
    case Enums::Normal:
        p->text->setText(p->txt);
        break;

    case Enums::ToDo:
        p->todo->setText(p->txt);
        break;
    }

    emit textChanged();
}

QString PaperTextArea::text() const
{
    return p->txt;
}

void PaperTextArea::setPlaceholderText(const QString &txt)
{
    p->placeholder = txt;
    if( p->text )
        p->text->setPlaceholderText(p->placeholder);
}

QString PaperTextArea::placeholderText() const
{
    return p->placeholder;
}

void PaperTextArea::setViewFont(const QFont &font)
{
    if( p->font == font )
        return;

    p->font = font;
    switch( p->type )
    {
    case Enums::Normal:
        p->text->setFont(p->font);
        break;

    case Enums::ToDo:
        p->todo->setFont(p->font);
        break;
    }

    emit viewFontChanged();
}

QFont PaperTextArea::viewFont() const
{
    return p->font;
}

void PaperTextArea::text_changed()
{
    switch( p->type )
    {
    case Enums::Normal:
        p->txt = p->text->toPlainText();
        break;

    case Enums::ToDo:
        p->txt = p->todo->text();
        break;
    }

    emit textChanged();
}

PaperTextArea::~PaperTextArea()
{
    delete p;
}

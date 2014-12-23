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

#include "todopapersitem.h"

#include <QCheckBox>
#include <QTextEdit>
#include <QFontMetrics>
#include <QHBoxLayout>
#include <QEvent>
#include <QKeyEvent>
#include <QTextDocument>
#include <QApplication>
#include <QDebug>

class ToDoPapersItemPrivate
{
public:
    QCheckBox *check;
    QTextEdit *text;
    QHBoxLayout *layout;
};

ToDoPapersItem::ToDoPapersItem(QWidget *parent) :
    QWidget(parent)
{
    p = new ToDoPapersItemPrivate;

    p->check = new QCheckBox();
    p->check->setChecked(false);

    p->text = new QTextEdit();
    p->text->setFrameShape(QFrame::NoFrame);
    p->text->setFrameShadow(QFrame::Plain);
    p->text->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->text->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    p->text->setTextInteractionFlags(Qt::TextEditorInteraction);
    p->text->installEventFilter(this);

    p->layout = new QHBoxLayout(this);
    p->layout->addWidget(p->check);
    p->layout->addWidget(p->text);
    p->layout->setContentsMargins(0,0,0,0);
    p->layout->setSpacing(1);

    connect( p->text , SIGNAL(textChanged()), SLOT(refreshSize())   );
    connect( p->text , SIGNAL(textChanged()), SIGNAL(textChanged()) );
    connect( p->check, SIGNAL(clicked())    , SIGNAL(textChanged()) );

    refreshSize();
}

QString ToDoPapersItem::text() const
{
    return p->text->toPlainText();
}

void ToDoPapersItem::setText(const QString &txt)
{
    if( txt == p->text->toPlainText() )
        return;

    p->text->setText( txt );
    refreshSize();
}

void ToDoPapersItem::setViewFont(const QFont &font)
{
    if( p->text->font() == font )
        return;

    p->text->setFont(font);
    emit viewFontChanged();
}

QFont ToDoPapersItem::viewFont() const
{
    return p->text->font();
}

void ToDoPapersItem::setCheckable(bool stt)
{
    if( p->check->isCheckable() == stt )
        return;

    p->check->setVisible(stt);
    p->check->setCheckable(stt);

    if( !stt )
        p->text->setPlaceholderText(tr("Text..."));
    else
        p->text->setPlaceholderText(QString());

    emit checkableChanged();
}

bool ToDoPapersItem::isCheckable() const
{
    return p->check->isCheckable();
}

void ToDoPapersItem::setChecked(bool stt)
{
    if( p->check->isChecked() == stt )
        return;

    p->check->setChecked(stt);
    emit checkedChanged();
}

bool ToDoPapersItem::isChecked() const
{
    return p->check->isChecked();
}

void ToDoPapersItem::setTextFocus(bool stt)
{
    if( QApplication::focusWidget() == p->text )
        return;

    if( stt )
        p->text->setFocus();

    emit textFocusChanged();
}

bool ToDoPapersItem::textFocus() const
{
    return QApplication::focusWidget() == p->text;
}

int ToDoPapersItem::cursorPosition() const
{
    return p->text->textCursor().position();
}

void ToDoPapersItem::setCursorPosition(int pos)
{
    if( pos > text().length() )
        pos = text().length();

    QTextCursor cursor = p->text->textCursor();
    cursor.setPosition(pos);

    p->text->setTextCursor(cursor);
}

void ToDoPapersItem::refreshSize()
{
    QTextDocument *doc = p->text->document();
    int height = doc->size().height();
    if( height == 0 )
        height = QFontMetrics(p->text->font()).height();
    if( height < 26 )
        height = 26;

    setFixedHeight( height );
}

bool ToDoPapersItem::eventFilter(QObject *o, QEvent *e)
{
    if( o == p->text )
    {
        switch( static_cast<int>(e->type()) )
        {
        case QEvent::KeyPress:
        {
            QKeyEvent *ev = static_cast<QKeyEvent*>(e);
            if( ev->modifiers() != Qt::ShiftModifier && ev->key() == Qt::Key_Return )
            {
                emit accepted(this);
                return true;
            }
            if( ev->modifiers() == Qt::NoModifier && ev->key() == Qt::Key_Backspace )
            {
                QTextCursor cursor = p->text->textCursor();
                if( cursor.position() == 0 )
                {
                    emit removed(this);
                    return true;
                }
            }
            if( ev->modifiers() == Qt::NoModifier && ev->key() == Qt::Key_Delete )
            {
                QTextCursor cursor = p->text->textCursor();
                if( cursor.position() == p->text->toPlainText().length() )
                {
                    emit removeNext(this);
                    return true;
                }
            }
            if( ev->modifiers() == Qt::NoModifier && ev->key() == Qt::Key_Up )
            {
                emit upPressed(this);
                return true;
            }
            if( ev->modifiers() == Qt::NoModifier && ev->key() == Qt::Key_Down )
            {
                emit downPressed(this);
                return true;
            }
        }
            break;
        }
    }

    return QWidget::eventFilter(o,e);
}

ToDoPapersItem::~ToDoPapersItem()
{
    delete p;
}

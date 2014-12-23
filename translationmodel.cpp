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

#include "translationmodel.h"

#include <QHash>
#include <QMap>
#include <QFile>
#include <QRegExp>
#include <QList>
#include <QStringList>
#include <QDomDocument>
#include <QLocale>
#include <QDebug>

class TranslationModelPrivate
{
public:
    QMap<QString,QString> translations;
    QHash<QString, QHash<QString, QHash<quint32,QString> > > paths;
    int done_count;

    QString filePath;
    int language;
    int country;

    QDomDocument dom;
};

TranslationModel::TranslationModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new TranslationModelPrivate;
    p->done_count = 0;
    p->country = QLocale::AnyCountry;
    p->language = QLocale::AnyLanguage;
}

void TranslationModel::setFile(const QString &path)
{
    if( p->filePath == path )
        return;

    p->filePath = path;
    emit fileChanged();
    refresh();
}

QString TranslationModel::file() const
{
    return p->filePath;
}

void TranslationModel::setLanguage(int lan)
{
    if( p->language == lan )
        return;

    p->language = lan;
    emit languageChanged();
    emit localeNameChanged();
}

int TranslationModel::language() const
{
    return p->language;
}

void TranslationModel::setCountry(int cn)
{
    if( p->country == cn )
        return;

    p->country = cn;
    emit countryChanged();
    emit localeNameChanged();
}

int TranslationModel::country() const
{
    return p->country;
}

QString TranslationModel::localeName() const
{
    QLocale locale( static_cast<QLocale::Language>(p->language),
                    static_cast<QLocale::Country>(p->country) );

    return locale.name();
}

QString TranslationModel::id(const QModelIndex &index) const
{
    int row = index.row();
    QMapIterator<QString,QString> i(p->translations);
    while( row >= 0 && i.hasNext() )
    {
        i.next();
        row--;
    }

    return i.key();
}

int TranslationModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return p->translations.count();
}

QVariant TranslationModel::data(const QModelIndex &index, int role) const
{
    const QString & key = id(index);
    QVariant res;
    switch( role )
    {
    case Qt::DisplayRole:
    case OriginalTextRole:
        res = key;
        break;

    case TranslationRole:
        res = p->translations.value(key);
        break;
    }

    return res;
}

bool TranslationModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const QString & key = id(index);
    bool res;
    switch( role )
    {
    case Qt::EditRole:
    case TranslationRole:
        if( p->translations.value(key).trimmed().isEmpty() )
        {
            if( !value.toString().trimmed().isEmpty() )
                p->done_count++;
        }
        else
        {
            if( value.toString().trimmed().isEmpty() )
                p->done_count--;
        }

        p->translations[key] = value.toString().trimmed();
        emit doneCountChanged();
        res = true;
        break;
    }

    return res;
}

QHash<qint32, QByteArray> TranslationModel::roleNames() const
{
    QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( OriginalTextRole, "originalText" );
    res->insert( TranslationRole , "translation" );

    return *res;
}

Qt::ItemFlags TranslationModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int TranslationModel::count() const
{
    return p->translations.count();
}

int TranslationModel::doneCount() const
{
    return p->done_count;
}

QStringList TranslationModel::countries() const
{
    QStringList *res = 0;
    if( res )
        return *res;

    res = new QStringList();
    for( int i=QLocale::AnyCountry; i<=QLocale::LastCountry; i++ )
        *res << QLocale::countryToString( static_cast<QLocale::Country>(i) );

    return *res;
}

QStringList TranslationModel::languages() const
{
    QStringList *res = 0;
    if( res )
        return *res;

    res = new QStringList();
    for( int i=QLocale::AnyLanguage; i<=QLocale::LastLanguage; i++ )
        *res << QLocale::languageToString( static_cast<QLocale::Language>(i) );

    return *res;
}

void TranslationModel::refresh()
{
    clear();

    if( !QFile::exists(p->filePath) )
        return;

    QFile file(p->filePath);
    if( !file.open(QFile::ReadOnly) )
        return;

    QString errorStr;
    int errorLine;
    int errorColumn;

    if(!p->dom.setContent(&file, true, &errorStr, &errorLine, &errorColumn))
    {
        qDebug() << QString("Parse error at line %1, column %2:\n%3").arg(errorLine).arg(errorColumn).arg(errorStr);
        return;
    }

    QDomElement root = p->dom.documentElement();
    if(root.tagName() != "TS")
    {
        qDebug() << QString("The file is not an TS file.");
        return;
    }
    else
    if(root.hasAttribute("version") && root.attribute("version") < "2.0")
    {
        qDebug() << QString("The file has old version.");
        return;
    }

    QLocale locale(root.attribute("language"));

    setLanguage( locale.language() );
    setCountry( locale.country() );

    QMap<QString,QString> translations;
    QDomElement child = root.firstChildElement("context");
    while (!child.isNull())
    {
        translations.unite( parseFolderElement(child) );
        child = child.nextSiblingElement("context");
    }

    beginInsertRows(QModelIndex(),0,translations.count());
    QMapIterator<QString,QString> i(translations);
    while( i.hasNext() )
    {
        i.next();
        bool contain_before = p->translations.contains(i.key());
        p->translations[i.key()] = i.value();

        emit countChanged();
        if( !i.value().trimmed().isEmpty()&& !contain_before )
        {
            p->done_count++;
            emit doneCountChanged();
        }
    }

    endInsertRows();
}

void TranslationModel::saveTo(const QString &path)
{
    QDomDocument doc("TS");

    QLocale locale( static_cast<QLocale::Language>(p->language),
                    static_cast<QLocale::Country>(p->country) );

    QDomElement root = doc.createElement("TS");
    root.setAttribute("version","2.0");
    root.setAttribute("language",locale.name());
    doc.appendChild(root);

    QHashIterator<QString, QHash<QString, QHash<quint32,QString> > > ci(p->paths); // Context Iterator
    while( ci.hasNext() )
    {
        ci.next();

        QDomElement context = doc.createElement("context");
        root.appendChild(context);

        QDomElement cname = doc.createElement("name");
        context.appendChild(cname);

        QDomText cname_txt = doc.createTextNode(ci.key());
        cname.appendChild(cname_txt);

        QHashIterator<QString, QHash<quint32,QString> > mi(ci.value());
        while( mi.hasNext() )
        {
            mi.next();

            QHashIterator<quint32,QString> li(mi.value());
            while( li.hasNext() )
            {
                li.next();
                const QString & original_text = li.value();
                const QString & translation_text = p->translations.value(original_text);

                QDomElement message = doc.createElement("message");
                context.appendChild(message);

                QDomElement location = doc.createElement("location");
                location.setAttribute("filename", mi.key());
                location.setAttribute("line", li.key());
                message.appendChild(location);

                QDomElement source = doc.createElement("source");
                message.appendChild(source);

                QDomText source_txt = doc.createTextNode(original_text);
                source.appendChild(source_txt);

                QDomElement translation = doc.createElement("translation");
                if( translation_text.isEmpty() )
                    translation.setAttribute("type", "unfinished");
                message.appendChild(translation);

                QDomText tr_txt = doc.createTextNode(translation_text);
                translation.appendChild(tr_txt);
            }
        }
    }

    QFile file(path);
    if( !file.open(QFile::WriteOnly) )
        return;

    file.write( doc.toByteArray(4) );
    file.close();
}

QMap<QString,QString> TranslationModel::parseFolderElement(const QDomElement &element)
{
    QMap<QString,QString> translations;
    const QString & name = element.firstChildElement("name").text();

    QDomElement child = element.firstChildElement();
    while (!child.isNull())
    {
        if (child.tagName() == "context")
        {
            parseFolderElement(child);
        }
        else
        if (child.tagName() == "message")
        {
            const QDomElement & domLocation = child.firstChildElement("location");
            const QString & fileName = domLocation.attribute("filename");
            const QString & lineNumber = domLocation.attribute("line");

            const QString & originalText = child.firstChildElement("source").text();
            const QString & translatedText = child.firstChildElement("translation").text();

            p->paths[name][fileName][lineNumber.toUInt()] = originalText;
            translations[originalText] = translatedText;
        }

        child = child.nextSiblingElement();
    }

    return translations;
}

void TranslationModel::clear()
{
    beginRemoveRows(QModelIndex(),0,p->translations.count());

    QMapIterator<QString,QString> i(p->translations);
    while( i.hasNext() )
    {
        i.next();
        p->translations.remove(i.key());

        emit countChanged();
        if( !i.value().trimmed().isEmpty() && !p->translations.contains(i.key()) )
        {
            p->done_count--;
            emit doneCountChanged();
        }
    }

    p->paths.clear();

    endRemoveRows();
}

TranslationModel::~TranslationModel()
{
    delete p;
}

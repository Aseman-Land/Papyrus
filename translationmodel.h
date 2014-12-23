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

#ifndef TRANSLATIONMODEL_H
#define TRANSLATIONMODEL_H

#include <QAbstractListModel>
#include <QMap>
#include <QHash>

class QDomElement;
class TranslationModelPrivate;
class TranslationModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString file READ file WRITE setFile NOTIFY fileChanged)
    Q_PROPERTY(QString localeName READ localeName NOTIFY localeNameChanged)
    Q_PROPERTY(int language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(int country READ country WRITE setCountry NOTIFY countryChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(int doneCount READ doneCount NOTIFY doneCountChanged)

public:
    enum TranslationRoles {
        OriginalTextRole = Qt::UserRole,
        TranslationRole
    };

    TranslationModel(QObject *parent = 0);
    ~TranslationModel();

    void setFile( const QString & path );
    QString file() const;

    void setLanguage( int lan );
    int language() const;

    void setCountry( int cn );
    int country() const;

    QString localeName() const;

    QString id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);

    QHash<qint32,QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;

    int count() const;
    int doneCount() const;

    Q_INVOKABLE QStringList countries() const;
    Q_INVOKABLE QStringList languages() const;

public slots:
    void refresh();
    void saveTo( const QString & path );

signals:
    void fileChanged();
    void languageChanged();
    void countryChanged();
    void localeNameChanged();
    void countChanged();
    void doneCountChanged();

private:
    QMap<QString, QString> parseFolderElement(const QDomElement &element);

private slots:
    void clear();

private:
    TranslationModelPrivate *p;
};

#endif // TRANSLATIONMODEL_H

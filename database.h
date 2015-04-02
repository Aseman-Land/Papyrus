/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Papyrus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Papyrus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DATABASE_H
#define DATABASE_H

#define GROUPS_SYNC_KEY "Groups"
#define NO_GROUP_UUID   "{47e4f990-c3db-4077-bb22-81172160986b}"

#include "databasedatatypes.h"

#include <QObject>
#include <QDate>
#include <QRectF>
#include <QList>
#include <QHash>
#include <QVariant>

class QSqlQuery;
class DatabasePrivate;
class Database : public QObject
{
    Q_OBJECT
    Q_ENUMS(PasswordType)

public:
    enum PasswordType {
        Numeric    = 0,
        Characters = 1
    };

    Database(QObject *parent = 0);
    ~Database();

    Q_INVOKABLE void connect();
    Q_INVOKABLE void disconnect();

    Q_INVOKABLE void setPath( const QString & path );
    Q_INVOKABLE QString path() const;

    Q_INVOKABLE void setCurrentActivity( int activity );
    Q_INVOKABLE int currentActivity() const;

    Q_INVOKABLE QList<int> dates();
    Q_INVOKABLE QList<int> lastThreeDays();

    Q_INVOKABLE int papersCountOf( const QDate & date );
    Q_INVOKABLE QList<int> papersOf( const QDate & date );
    Q_INVOKABLE QList<int> papersOf( int group_id );
    Q_INVOKABLE QList<int> papers();
    Q_INVOKABLE QList<int> search(const QString & keyword );
    Q_INVOKABLE QList<int> advanceSearch(const QString & keyword, const QDate & startDate, const QDate & endDate, const QTime & startTime, const QTime & endTime, int group, int domain, const QRectF &geo = QRectF(), const QString & weather = QString() );

    Q_INVOKABLE int createPaper(const QString &uuid = QString());
    Q_INVOKABLE void deletePaper( int id );

    Q_INVOKABLE void setPaper(int id, const QString & title, const QString & text, int group );
    Q_INVOKABLE void setPaper(const QString & uuid, const QString & title, const QString & text, const QString & group, const QString & date, const QGeoCoordinate & location , int type, const QString & weather, int temperature);

    Q_INVOKABLE QString paperTitle( int id );
    Q_INVOKABLE void setPaperTitle( int id, const QString & title );

    Q_INVOKABLE QString paperText( int id );
    Q_INVOKABLE void setPaperText( int id, const QString & text );

    Q_INVOKABLE int paperType( int id );
    Q_INVOKABLE void setPaperType( int id, int type );

    Q_INVOKABLE int paperTemperature( int id );
    Q_INVOKABLE void setPaperTemperature( int id, int tmp );

    Q_INVOKABLE QString paperWeather( int id );
    Q_INVOKABLE void setPaperWeather( int id, const QString & weather );

    Q_INVOKABLE QGeoCoordinate paperLocation( int id );
    Q_INVOKABLE void setPaperLocation( int id, const QGeoCoordinate & coo );
    Q_INVOKABLE void setPaperLocation(int id, const QPointF & point );

    Q_INVOKABLE int paperGroup( int id );
    Q_INVOKABLE void setPaperGroup( int id, int group );

    Q_INVOKABLE QDateTime paperCreatedDate( int id );
    Q_INVOKABLE void setPaperCreatedDate( int id, const QDateTime & dt );

    Q_INVOKABLE QDateTime paperModifiedDate( int id );
    Q_INVOKABLE QString paperUuid( int id );
    Q_INVOKABLE int paperActivity( int id );
    Q_INVOKABLE int paperUuidId( const QString & uuid );

    Q_INVOKABLE QStringList paperFiles( int id );
    Q_INVOKABLE bool hasFiles( int id );
    Q_INVOKABLE void addFileToPaper( int id, const QString & file );
    Q_INVOKABLE void addCustomFileToPaper( int id, const QString & file, qint64 date );
    Q_INVOKABLE void removeFileFromPaper( int id, const QString & file );
    Q_INVOKABLE bool fileContaintOnDatabase( const QString & file );
    Q_INVOKABLE qint64 fileTime( int id, const QString & file );

    Q_INVOKABLE QList<int> groups();
    Q_INVOKABLE bool groupIsDeleted( int id );
    Q_INVOKABLE int createGroup(const QString &uuid = QString());
    Q_INVOKABLE void deleteGroup( int id );
    Q_INVOKABLE void setGroup( const QString & uuid, const QString & name, const QString & color );

    Q_INVOKABLE QString groupName( int id );
    Q_INVOKABLE void setGroupName( int id, const QString & name );

    Q_INVOKABLE QColor groupColor( int id );
    Q_INVOKABLE void setGroupColor(int id , const QColor &color);

    Q_INVOKABLE QString groupUuid( int id );
    Q_INVOKABLE int groupUuidId( const QString & uuid );

    Q_INVOKABLE int groupPapersCount( int id );

    Q_INVOKABLE QList<int> activities();

    Q_INVOKABLE int passwordType();
    Q_INVOKABLE QString password();
    Q_INVOKABLE void setPassword(const QString & pass, int type = Numeric );

    Q_INVOKABLE QString syncPassword();
    Q_INVOKABLE void setSyncPassword( const QString & pass );

    Q_INVOKABLE QString value( const QString & property );
    Q_INVOKABLE void setValue( const QString & property, const QString & value );

    Q_INVOKABLE int statePapersCount();
    Q_INVOKABLE int stateDatesCount();
    Q_INVOKABLE int stateGroupsCount();

    Q_INVOKABLE void setRevision( const QString & id, int revisionOf );
    Q_INVOKABLE int revisionOf( const QString & id ) const;
    Q_INVOKABLE SyncItemHash revisions() const;

public slots:
    void setSignalBlocker(bool stt );

    void begin();
    void commit();

signals:
    void currentActivityChanged();
    void groupsListChanged();
    void datesListChanged();
    void activityListChanged();
    void revisionsChanged();
    void revisionChanged( const QString &iid, int revision );
    void filesListChanged();

    void paperChanged( int id );
    void paperGroupChanged( int id );
    void fileDeleted( const QString & file );

    void groupNameChanged(int id);
    void groupColorChanged(int id);
    void groupPapersCountChanged(int id);

private:
    void reinit_buffer();
    void update_database();

    PaperClass getPaper( int id );
    void savePaper(PaperClass paper);
    void saveGroup(GroupClass group);
    void saveActivity(ActivityClass activity);

    QList<int> searchQuery(const QString &query , const QString &keyword, const QHash<QString, QVariant> &boundValues = QHash<QString, QVariant>());

protected:
    void timerEvent(QTimerEvent *e);

private:
    DatabasePrivate *p;
};

#endif // DATABASE_H

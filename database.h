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

public slots:
    void connect();
    void disconnect();

    void setPath( const QString & path );
    QString path() const;

    void setCurrentActivity( int activity );
    int currentActivity() const;

    QList<int> dates();
    QList<int> lastThreeDays();

    int papersCountOf( const QDate & date );
    QList<int> papersOf( const QDate & date );
    QList<int> papersOf( int group_id );
    QList<int> papers();
    QList<int> search(const QString & keyword );
    QList<int> advanceSearch(const QString & keyword, const QDate & startDate, const QDate & endDate, const QTime & startTime, const QTime & endTime, int group, int domain, const QRectF &geo = QRectF(), const QString & weather = QString() );

    int createPaper(const QString &uuid = QString());
    void deletePaper( int id );

    void setPaper(int id, const QString & title, const QString & text, int group );
    void setPaper(const QString & uuid, const QString & title, const QString & text, const QString & group, const QString & date, const QGeoCoordinate & location , int type, const QString & weather, int temperature);

    QString paperTitle( int id );
    void setPaperTitle( int id, const QString & title );

    QString paperText( int id );
    void setPaperText( int id, const QString & text );

    int paperType( int id );
    void setPaperType( int id, int type );

    int paperTemperature( int id );
    void setPaperTemperature( int id, int tmp );

    QString paperWeather( int id );
    void setPaperWeather( int id, const QString & weather );

    QGeoCoordinate paperLocation( int id );
    void setPaperLocation( int id, const QGeoCoordinate & coo );
    void setPaperLocation(int id, const QPointF & point );

    int paperGroup( int id );
    void setPaperGroup( int id, int group );

    QDateTime paperCreatedDate( int id );
    void setPaperCreatedDate( int id, const QDateTime & dt );

    QDateTime paperModifiedDate( int id );
    QString paperUuid( int id );
    int paperActivity( int id );
    int paperUuidId( const QString & uuid );

    QStringList paperFiles( int id );
    bool hasFiles( int id );
    void addFileToPaper( int id, const QString & file );
    void addCustomFileToPaper( int id, const QString & file, qint64 date );
    void removeFileFromPaper( int id, const QString & file );
    bool fileContaintOnDatabase( const QString & file );
    qint64 fileTime( int id, const QString & file );

    QList<int> groups();
    bool groupIsDeleted( int id );
    int createGroup(const QString &uuid = QString());
    void deleteGroup( int id );
    void setGroup( const QString & uuid, const QString & name, const QString & color );

    QString groupName( int id );
    void setGroupName( int id, const QString & name );

    QColor groupColor( int id );
    void setGroupColor(int id , const QColor &color);

    QString groupUuid( int id );
    int groupUuidId( const QString & uuid );

    int groupPapersCount( int id );

    QList<int> activities();

    int passwordType();
    QString password();
    void setPassword(const QString & pass, int type = Numeric );

    QString syncPassword();
    void setSyncPassword( const QString & pass );

    QString value( const QString & property );
    void setValue( const QString & property, const QString & value );

    int statePapersCount();
    int stateDatesCount();
    int stateGroupsCount();

    void setRevision( const QString & id, int revisionOf );
    int revisionOf( const QString & id ) const;
    SyncItemHash revisions() const;

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
    void paperCreated(int id);
    void paperDeleted(int id);

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

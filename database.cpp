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

#include "papyrusmacros.h"

#define CACHE_SIZE 20
#define DB_EMIT if( !p->signal_blocker ) emit
#define BEGIN \
    BEGIN_FNC_DEBUG \
    begin();
#define COMMIT \
    if( p->commit_timer ) {\
        killTimer(p->commit_timer); \
        p->commit_timer = 0; \
    } \
    p->uncommited++; \
    if( p->uncommited > 40 ) \
        commit(); \
    else \
        p->commit_timer = startTimer(800); \
    END_FNC_DEBUG

#include "database.h"
#include "papyrusmacros.h"
#include "papyrus.h"
#include "asemandeveloper_macros.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>
#include <QDebug>
#include <QHash>
#include <QVariant>
#include <QStringList>
#include <QMap>
#include <QUuid>
#include <QCoreApplication>
#include <QTimerEvent>

class DatabasePrivate
{
public:
    QSqlDatabase db;
    QString password;

    QMap<int,GroupClass> groups;
    QMap<int,ActivityClass> activities;
    QList<PaperClass> papers;
    SyncItemHash syncs;
    QSet<int> cached_papers_ids;

    int current_activity;
    int max_paper_id;

    QHash<QString,QString> general;
    QString path;

    bool signal_blocker;
    bool begined;
    int commit_timer;
    int uncommited;
};

Database::Database(QObject *parent) :
    QObject(parent)
{
    p = new DatabasePrivate;
    p->current_activity = 0;
    p->max_paper_id = 0;
    p->path = Papyrus::instance()->profilePath() + "/database.sqlite";
    p->signal_blocker = false;
    p->begined = false;
    p->commit_timer = 0;
    p->uncommited = 0;

    qRegisterMetaType<SyncItemHash>("SyncItemHash");
    qRegisterMetaType<SyncItem>("SyncItem");

    if( !Papyrus::settings()->value("initialize/userdata_db",false).toBool() )
    {
        QFile::copy(":/files/database/database.sqlite",p->path);
        QFile(p->path).setPermissions(QFileDevice::WriteOwner|QFileDevice::WriteGroup|QFileDevice::ReadUser|QFileDevice::ReadGroup);
    }

    Papyrus::settings()->setValue("initialize/userdata_db",true);

    p->db = QSqlDatabase::addDatabase( "QSQLITE", DATA_DB_CONNECTION );
    setPath(p->path);
}

void Database::connect()
{
    if( p->db.isOpen() )
        return;

    p->db.open();
    reinit_buffer();
    update_database();
}

void Database::disconnect()
{
    if( !p->db.isOpen() )
        return;

    p->db.close();
}

void Database::setPath(const QString &path)
{
    p->path = path;
    disconnect();
    p->db.setDatabaseName(p->path);
    connect();
}

QString Database::path() const
{
    return p->path;
}

void Database::setCurrentActivity(int activity)
{
    if( activity == p->current_activity )
        return;

    p->current_activity = activity;
    DB_EMIT currentActivityChanged();
}

int Database::currentActivity() const
{
    return p->current_activity;
}

void Database::begin()
{
    if( p->begined )
        return;

    QSqlQuery query( p->db );
    query.prepare( "BEGIN" );
    query.exec();
    p->begined = true;
}

void Database::commit()
{
    if( !p->begined )
        return;

    QSqlQuery query( p->db );
    query.prepare( "COMMIT" );
    query.exec();
    p->uncommited = 0;
    p->begined = false;
}

QList<int> Database::dates()
{
    QSqlQuery query(p->db);
    query.prepare("SELECT cdate FROM Papers");
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    QSet<int> res_set;
    while( query.next() )
    {
        QSqlRecord record = query.record();
        res_set << record.value(0).toInt();
    }

    QList<int> _result = res_set.toList();
    qSort(_result.begin(),_result.end());

    QList<int> result;
    foreach( int r, _result )
        result.prepend(r);

    return result;
}

QList<int> Database::lastThreeDays()
{
    int current_day = QDate(1,1,1).daysTo(QDate::currentDate());

    const QSet<int> & tmp_res = dates().toSet();

    QList<int> result;
    if( tmp_res.contains(current_day) )
        result << current_day;
    if( tmp_res.contains(current_day-1) )
        result << current_day-1;
    if( tmp_res.contains(current_day-2) )
        result << current_day-2;

    return result;
}

int Database::papersCountOf(const QDate &date)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT count(id) FROM Papers WHERE cdate=:date");
    query.bindValue(":date",QDate(1,1,1).daysTo(date));
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    if( !query.next() )
        return 0;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

QList<int> Database::papersOf(const QDate &date)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT id,ctime FROM Papers WHERE cdate=:date");
    query.bindValue(":date",QDate(1,1,1).daysTo(date));
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    QMap<int,int> result;
    while( query.next() )
    {
        QSqlRecord record = query.record();
        int id    = record.value(0).toInt();
        int ctime = record.value(1).toInt();

        result.insertMulti( ctime, id );
    }

    return result.values();
}

QList<int> Database::papersOf(int group_id)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT id,ctime,cdate FROM Papers WHERE grp=:grp");
    query.bindValue(":grp",group_id);
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    QMap<qint64,int> result;
    while( query.next() )
    {
        QSqlRecord record = query.record();
        int id    = record.value(0).toInt();
        int ctime = record.value(1).toInt();
        int cdate = record.value(2).toInt();

        result.insertMulti( cdate*3600*25+ctime, id );
    }

    return result.values();
}

QList<int> Database::papers()
{
    QSqlQuery query(p->db);
    query.prepare("SELECT id,ctime,cdate FROM Papers");
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    QMap<int,int> result;
    while( query.next() )
    {
        QSqlRecord record = query.record();
        int id    = record.value(0).toInt();
        int ctime = record.value(1).toInt();
        int cdate = record.value(2).toInt();

        result.insertMulti( -(cdate*3600*25+ctime), id );
    }

    return result.values();
}

QHash<int,qint64> *search_sort_result;
QHash<int,int> *search_sort_result_counts;
QSet<int> *search_sort_result_exact;
bool searchSort( int a, int b )
{
    QHash<int,qint64> & result = *search_sort_result;
    QHash<int,int> & result_counts = *search_sort_result_counts;
    QSet<int> & result_exact = *search_sort_result_exact;
    if( result_exact.contains(a) && !result_exact.contains(b) )
        return true;

    const int a_res_cnt = result_counts[a];
    const int b_res_cnt = result_counts[b];
    if( a_res_cnt > b_res_cnt )
        return true;
    if( a_res_cnt < b_res_cnt )
        return false;

    const qint64 a_data = result[a];
    const qint64 b_data = result[b];
    if( a_data > b_data )
        return true;
    if( a_data < b_data )
        return false;

    return a>b;
}

QList<int> Database::search(const QString &keyword)
{
    QString query_str = "SELECT id,ctime,cdate FROM Papers WHERE title LIKE :fkwrd OR text LIKE :skwrd";
    return searchQuery(query_str,keyword);
}

QList<int> Database::advanceSearch(const QString &keyword, const QDate &startDate, const QDate &endDate, const QTime &startTime, const QTime &endTime, int group, int domain, const QRectF & geo, const QString & weather )
{
    BEGIN_FNC_DEBUG
    QString query_str = "SELECT id,ctime,cdate FROM Papers WHERE ";
    QHash<QString,QVariant> boundValues;

    bool has_previous = false;
    if( !keyword.isEmpty() )
    {
        query_str += "(title LIKE :fkwrd OR text LIKE :skwrd)";
        has_previous = true;
    }
    if( !startDate.isNull() )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "cdate>=:csdate";
        boundValues.insert(":csdate",QDate(1,1,1).daysTo(startDate));
        has_previous = true;
    }
    if( !endDate.isNull() )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "cdate<=:cedate";
        boundValues.insert(":cedate",QDate(1,1,1).daysTo(endDate));
        has_previous = true;
    }
    if( !startTime.isNull() )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "ctime>=:cstime";
        boundValues.insert(":cstime",QTime(0,0,0).secsTo(startTime));
        has_previous = true;
    }
    if( !endTime.isNull() )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "ctime<=:cetime";
        boundValues.insert(":cetime",QTime(0,0,0).secsTo(endTime));
        has_previous = true;
    }
    if( group != -1 )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "grp=:grp";
        boundValues.insert(":grp",group);
        has_previous = true;
    }
    if( !weather.isEmpty() )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "weather=:wthr";
        boundValues.insert(":wthr",weather);
        has_previous = true;
    }
    if( geo.isValid() )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += ":llttd<latitude AND latitude<:blttd";
        boundValues.insert(":llttd",geo.y());
        boundValues.insert(":blttd",geo.y()+geo.height());

        query_str += " AND ";

        query_str += ":llntd<longitude AND longitude<:blntd";
        boundValues.insert(":llntd",geo.x());
        boundValues.insert(":blntd",geo.x()+geo.width());

        has_previous = true;
    }
    if( domain != Enums::AllPapers )
    {
        if( has_previous )
            query_str += " AND ";

        query_str += "type=:type";
        switch( domain )
        {
        case Enums::NormalPapers:
            boundValues.insert(":type",static_cast<int>(Enums::Normal));
            break;
        case Enums::ToDoPapers:
            boundValues.insert(":type",static_cast<int>(Enums::ToDo));
            break;

        case Enums::UncompletedTasks:
            query_str += " AND text LIKE :kwd";
            boundValues.insert(":kwd","%- %");
            boundValues.insert(":type",static_cast<int>(Enums::ToDo));
            break;
        case Enums::CompletedTasks:
            query_str += " AND text LIKE :kwd";
            boundValues.insert(":kwd","%* %");
            boundValues.insert(":type",static_cast<int>(Enums::ToDo));
            break;
        }
        has_previous = true;
    }

    END_FNC_DEBUG
    if( !has_previous )
        return QList<int>();

    return searchQuery(query_str,keyword,boundValues);
}

QList<int> Database::searchQuery(const QString &qry_str, const QString & kw, const QHash<QString, QVariant> & boundValues)
{
    BEGIN_FNC_DEBUG
    QHash<int,qint64> result;
    QHash<int,int> result_counts;
    QSet<int> result_exact;

    QStringList keywords = kw.split(" ",QString::SkipEmptyParts);
    keywords.prepend(kw);

    bool first_done = false;
    foreach( const QString & keyword, keywords )
    {
        QSqlQuery query(p->db);
        query.prepare(qry_str);
        query.bindValue(":fkwrd","%"+keyword+"%");
        query.bindValue(":skwrd","%"+keyword+"%");

        QHashIterator<QString, QVariant> bvi(boundValues);
        while( bvi.hasNext() )
        {
            bvi.next();
            query.bindValue(bvi.key(),bvi.value());
        }

        query.exec();
        if( query.lastError().isValid() )
            qDebug() << query.lastError() << query.lastQuery() << query.boundValues();

        while( query.next() )
        {
            QSqlRecord record = query.record();
            int id = record.value(0).toInt();
            int ctime = record.value(1).toInt();
            int cdate = record.value(2).toInt();

            result_counts[id]++;
            if( result.contains(id) )
                continue;

            result.insert( id, cdate*3600*25+ctime );
            if( !first_done )
                result_exact.insert(id);
        }

        first_done = true;
    }

    search_sort_result = &result;
    search_sort_result_counts = &result_counts;
    search_sort_result_exact = &result_exact;
    QList<int> final_result = result.keys();
    qSort(final_result.begin(),final_result.end(),searchSort);

    END_FNC_DEBUG
    return final_result;
}

int Database::createPaper( const QString & uuid )
{
    PaperClass paper;
    paper.id       = p->max_paper_id;
    paper.uuid     = uuid.isEmpty()? QUuid::createUuid().toString() : uuid;
    paper.create   = QDateTime::currentDateTime();
    paper.group    = 0;
    paper.activity = 0;

    p->max_paper_id++;

    savePaper(paper);

    emit datesListChanged();
    return paper.id;
}

void Database::deletePaper(int id)
{
    PaperClass paper = getPaper(id);

    QSqlQuery query(p->db);
    query.prepare("DELETE FROM Papers WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();

    if( p->cached_papers_ids.contains(id) )
    {
        for( int i=0; i<p->papers.count(); i++ )
        {
            PaperClass & ppr = p->papers[i];
            if( ppr.id != id )
                continue;

            p->papers.removeAt(i);
            break;
        }

        p->cached_papers_ids.remove(id);
    }

    const QStringList & files = paperFiles(id);
    foreach( const QString & f, files )
        removeFileFromPaper(id,f);

    setRevision(paper.uuid,-2);
    emit paperGroupChanged(id);
}

void Database::setPaper(int id, const QString &title, const QString &text, int group)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.title == title && paper.text == text && paper.group == group )
        return;

    paper.title     = title;
    paper.text      = text;
    paper.group     = group;
    savePaper(paper);
    emit paperGroupChanged(id);
}

void Database::setPaper(const QString &uuid, const QString &title, const QString &text, const QString &group, const QString &date, const QGeoCoordinate & location, int type, const QString & weather, int temperature)
{
    int paper_id = paperUuidId(uuid);
    if( paper_id == -1 )
        paper_id = createPaper(uuid);

    int group_id = groupUuidId(group);
    if( group_id == -1 )
        group_id = createGroup(group);

    PaperClass paper = getPaper(paper_id);

    paper.uuid        = uuid;
    paper.title       = title;
    paper.text        = text;
    paper.group       = group_id;
    paper.location    = location;
    paper.create      = QDateTime::fromString(date);
    paper.type        = static_cast<Enums::PaperType>(type);
    paper.weather     = weather;
    paper.temperature = temperature;

    savePaper(paper);
    emit datesListChanged();
    emit paperGroupChanged(paper_id);
}

QString Database::paperTitle(int id)
{
    return getPaper(id).title;
}

void Database::setPaperTitle(int id, const QString &title)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.title == title )
        return;

    paper.title = title;
    savePaper(paper);
}

QString Database::paperText(int id)
{
    return getPaper(id).text;
}

void Database::setPaperText(int id, const QString &text)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.text == text )
        return;

    paper.text = text;
    savePaper(paper);
}

int Database::paperType(int id)
{
    return getPaper(id).type;
}

void Database::setPaperType(int id, int type)
{
    PaperClass paper = getPaper(id);
    if( type == -1 )
        return;
    if( paper.type == type )
        return;

    paper.type = static_cast<Enums::PaperType>(type);
    savePaper(paper);
}

int Database::paperTemperature(int id)
{
    return getPaper(id).temperature;
}

void Database::setPaperTemperature(int id, int tmp)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.temperature == tmp )
        return;

    paper.temperature = tmp;
    savePaper(paper);
}

QString Database::paperWeather(int id)
{
    return getPaper(id).weather;
}

void Database::setPaperWeather(int id, const QString &weather)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.weather == weather )
        return;

    paper.weather = weather;
    savePaper(paper);
}

int Database::paperGroup(int id)
{
    return getPaper(id).group;
}

void Database::setPaperGroup(int id, int group)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.group == group )
        return;

    int old_group = paper.group;
    paper.group = group;
    savePaper(paper);

    emit paperGroupChanged(id);
    emit groupPapersCountChanged(old_group);
    emit groupPapersCountChanged(paper.group);
}

QDateTime Database::paperCreatedDate(int id)
{
    return getPaper(id).create;
}

void Database::setPaperCreatedDate(int id, const QDateTime &dt)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.create == dt )
        return;

    paper.create = dt;
    savePaper(paper);
}

QDateTime Database::paperModifiedDate(int id)
{
    return getPaper(id).modified;
}

QString Database::paperUuid(int id)
{
    return getPaper(id).uuid;
}

int Database::paperActivity(int id)
{
    return getPaper(id).activity;
}

QGeoCoordinate Database::paperLocation(int id)
{
    return getPaper(id).location;
}

void Database::setPaperLocation(int id, const QGeoCoordinate &coo)
{
    PaperClass paper = getPaper(id);
    if( paper.id == -1 )
        return;
    if( paper.location == coo )
        return;

    paper.location = coo;
    savePaper(paper);
}

void Database::setPaperLocation(int id, const QPointF &point)
{
    setPaperLocation( id, QGeoCoordinate(point.y(), point.x()) );
}

int Database::paperUuidId(const QString &uuid)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT id FROM Papers WHERE uuid=:uuid");
    query.bindValue(":uuid",uuid);
    query.exec();

    if( !query.next() )
        return -1;

    return query.record().value(0).toInt();
}

QStringList Database::paperFiles(int id)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT file,adate,atime FROM Files WHERE paper=:id");
    query.bindValue(":id",id);
    query.exec();

    QMap<qint64,QString> result;
    while( query.next() )
    {
        QSqlRecord record = query.record();
        int adate = record.value(1).toInt();
        int atime = record.value(2).toInt();

        result.insertMulti( adate*3600*25+atime, record.value(0).toString() );
    }

    return result.values();
}

bool Database::hasFiles(int id)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT file FROM Files WHERE paper=:id");
    query.bindValue(":id",id);
    query.exec();

    return query.next();
}

void Database::addFileToPaper(int id, const QString &file)
{
    const QDateTime & modified = QDateTime::currentDateTime();
    addCustomFileToPaper( id, file, QDateTime(QDate(1,1,1),QTime(0,0,0)).secsTo(modified) );
}

void Database::addCustomFileToPaper(int id, const QString &file, qint64 date)
{
    BEGIN
    const QDateTime & modified = QDateTime(QDate(1,1,1),QTime(0,0,0)).addSecs(date);

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Files (paper,file,adate,atime) VALUES (:id,:file,:adate,:atime)");
    query.bindValue(":id"   , id);
    query.bindValue(":file" , file);
    query.bindValue(":adate",QDate(1,1,1).daysTo(modified.date()));
    query.bindValue(":atime",QTime(0,0,0).secsTo(modified.time()));
    query.exec();

    DB_EMIT filesListChanged();
    COMMIT
}

void Database::removeFileFromPaper(int id, const QString &file)
{
    BEGIN
    QSqlQuery query(p->db);
    query.prepare("DELETE FROM Files WHERE paper=:id AND file=:file");
    query.bindValue(":id"   , id);
    query.bindValue(":file" , file);
    query.exec();

    bool containt = fileContaintOnDatabase(file);
    if( !containt )
        DB_EMIT fileDeleted(file);

    DB_EMIT filesListChanged();
    COMMIT
}

bool Database::fileContaintOnDatabase(const QString &file)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT paper FROM Files WHERE file=:file");
    query.bindValue(":file",file);
    query.exec();

    return query.next();
}

qint64 Database::fileTime(int id, const QString &file)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT adate,atime FROM Files WHERE paper=:id AND file=:file");
    query.bindValue(":id",id);
    query.bindValue(":file",file);
    query.exec();

    if( !query.next() )
        return 0;

    QSqlRecord record = query.record();
    int adate = record.value(1).toInt();
    int atime = record.value(2).toInt();

    QDateTime dt = QDateTime( QDate(1,1,1).addDays(adate), QTime(0,0,0).addSecs(atime) );
    return QDateTime(QDate(1,1,1),QTime(0,0,0)).secsTo(dt);
}

QList<int> Database::groups()
{
    QMap<QString,int> res;
    QMapIterator<int,GroupClass> i(p->groups);
    while( i.hasNext() )
    {
        i.next();
        if( i.key() == 0 )
            continue;

        res.insertMulti( i.value().name, i.key() );
    }

    QList<int> result = res.values();
    result.prepend(0);

    return result;
}

bool Database::groupIsDeleted(int id)
{
    if( !p->groups.contains(id) )
        return true;
    else
        return p->groups.value(id).name.isEmpty();
}

int Database::createGroup(const QString & uuid)
{
    GroupClass group;

    QMapIterator<int,GroupClass> gi(p->groups);
    while( gi.hasNext() )
    {
        gi.next();
        const GroupClass & gr = gi.value();
        if( gr.uuid != uuid )
            continue;

        group = gr;
    }

    if( group.uuid.isEmpty() )
    {
        group.id = p->groups.isEmpty()? 1 : p->groups.lastKey()+1;
        group.name = "";
        group.color = QColor("#000000");

        if( group.id == 0 )
            group.uuid = NO_GROUP_UUID;
        else
            group.uuid = uuid.isEmpty()? QUuid::createUuid().toString() : uuid;

        saveGroup(group);
    }

    return group.id;
}

void Database::deleteGroup(int id)
{
    if( !p->groups.contains(id) )
        return;

    GroupClass group = p->groups.value(id);
    group.name.clear();

    saveGroup(group);

    BEGIN
    QSqlQuery update_query( p->db );
    update_query.prepare( "UPDATE Papers SET grp=0 WHERE grp=:grp");
    update_query.bindValue( ":grp", id  );
    update_query.exec();

    for( int i=0; i<p->papers.count(); i++ )
        if( p->papers.at(i).group == id )
            p->papers[i].group = 0;
    COMMIT
}

void Database::setGroup(const QString &uuid, const QString &name, const QString &color)
{
    int id = groupUuidId(uuid);
    if( id == -1 )
        id = createGroup(uuid);

    GroupClass gr = p->groups.value(id);
    gr.color = color;
    gr.name = name.trimmed();
    gr.uuid = uuid;

    saveGroup(gr);

    emit groupNameChanged(id);
    emit groupColorChanged(id);
}

QString Database::groupName(int id)
{
    return p->groups.value(id).name;
}

void Database::setGroupName(int id, const QString &name)
{
    if( !p->groups.contains(id) )
        return;
    if( p->groups.value(id).name == name )
        return;

    p->groups[id].name = name.trimmed();
    saveGroup(p->groups[id]);
    emit groupNameChanged(id);
}

QColor Database::groupColor(int id)
{
    return p->groups.value(id).color;
}

void Database::setGroupColor(int id, const QColor & color)
{
    if( !p->groups.contains(id) )
        return;
    if( p->groups.value(id).color == color )
        return;

    p->groups[id].color = color;
    saveGroup(p->groups[id]);
    emit groupColorChanged(id);
}

QString Database::groupUuid(int id)
{
    return p->groups.value(id).uuid;
}

int Database::groupUuidId(const QString &uuid)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT id FROM Groups WHERE uuid=:uuid");
    query.bindValue(":uuid",uuid);
    query.exec();

    if( !query.next() )
        return -1;

    return query.record().value(0).toInt();
}

int Database::groupPapersCount(int id)
{
    QSqlQuery query(p->db);
    query.prepare("SELECT count(id) FROM Papers WHERE grp=:grp");
    query.bindValue(":grp",id);
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();
    if( !query.next() )
        return 0;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

QList<int> Database::activities()
{
    return p->activities.keys();
}

int Database::passwordType()
{
    return value("passType").toInt();
}

QString Database::password()
{
    return value("password");
}

void Database::setPassword(const QString &pass, int type)
{
    setValue("password",pass);
    setValue("passType",QString::number(type));
}

QString Database::syncPassword()
{
    return value("syncPassword");
}

void Database::setSyncPassword(const QString &pass)
{
    setValue("syncPassword",pass);
}

QString Database::value(const QString &property)
{
    return p->general.value(property);
}

void Database::setValue(const QString &property, const QString &value)
{
    BEGIN
    p->general[property] = value;

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO General (property,value) VALUES (:prp,:vlu)");
    query.bindValue(":prp", property);
    query.bindValue(":vlu", value);
    query.exec();
    COMMIT
}

int Database::statePapersCount()
{
    QSqlQuery query(p->db);
    query.prepare("SELECT count(id) FROM Papers");
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    if( !query.next() )
        return 0;

    QSqlRecord record = query.record();
    return record.value(0).toInt();
}

int Database::stateDatesCount()
{
    return dates().count();
}

int Database::stateGroupsCount()
{
    return p->groups.count();
}

void Database::setRevision(const QString &id, int revision)
{
    BEGIN
    SyncItem & sync = p->syncs[id];
    sync.id = id;
    if( sync.revision != revision )
    {
        sync.last_revision = sync.revision;
        sync.revision = revision;
    }

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Sync (id,revision,last_revision) VALUES (:id,:rsvn,:lrsvn)");
    query.bindValue(":id"   , sync.id);
    query.bindValue(":rsvn" , sync.revision);
    query.bindValue(":lrsvn", sync.last_revision);
    query.exec();

    DB_EMIT revisionsChanged();
    emit revisionChanged(id, revision);
    COMMIT
}

int Database::revisionOf(const QString &id) const
{
    return p->syncs.value(id).revision;
}

SyncItemHash Database::revisions() const
{
    return p->syncs;
}

void Database::setSignalBlocker(bool stt)
{
    p->signal_blocker = stt;
}

void Database::reinit_buffer()
{
    p->groups.clear();
    p->activities.clear();
    p->papers.clear();
    p->cached_papers_ids.clear();
    p->general.clear();
    p->current_activity = 0;
    p->max_paper_id = 0;

    QSqlQuery grp_query(p->db);
    grp_query.prepare("SELECT id,name,color,uuid FROM Groups");
    grp_query.exec();

    if( grp_query.lastError().isValid() )
        qDebug() << grp_query.lastError();


    GroupClass ungroup;
    ungroup.id    = 0;
    ungroup.name  = tr("No Label");
    ungroup.color = "#aaaaaa";
    p->groups[ungroup.id] = ungroup;

    while( grp_query.next() )
    {
        QSqlRecord record = grp_query.record();
        GroupClass group;
        group.id    = record.value(0).toInt();
        group.name  = record.value(1).toString();
        group.color = record.value(2).toString();
        group.uuid  = record.value(3).toString();

        p->groups[group.id] = group;
    }

    BEGIN
    QMapIterator<int,GroupClass> gi(p->groups);
    while( gi.hasNext() )
    {
        gi.next();
        GroupClass group = gi.value();
        if( !group.uuid.isEmpty() )
            continue;

        if( group.id == 0 )
            group.uuid = NO_GROUP_UUID;
        else
            group.uuid = QUuid::createUuid().toString();

        QSqlQuery query(p->db);
        query.prepare("INSERT OR REPLACE INTO Groups (id,name,color,uuid) VALUES (:id,:name,:color,:uuid)");
        query.bindValue(":id"   , group.id);
        query.bindValue(":name" , group.name);
        query.bindValue(":color", group.color.name());
        query.bindValue(":uuid" , group.uuid);
        query.exec();

        p->groups[group.id] = group;
    }
    COMMIT

    QSqlQuery actv_query(p->db);
    actv_query.prepare("SELECT id,description,image FROM Activity");
    actv_query.exec();

    if( actv_query.lastError().isValid() )
        qDebug() << actv_query.lastError();

    while( actv_query.next() )
    {
        QSqlRecord record = actv_query.record();
        ActivityClass activity;
        activity.id    = record.value(0).toInt();
        activity.name  = record.value(1).toString();
        activity.description = record.value(2).toString();
        activity.image = record.value(3).toString();

        p->activities[activity.id] = activity;
    }

    QSqlQuery gnrl_query(p->db);
    gnrl_query.prepare("SELECT property,value FROM General");
    gnrl_query.exec();

    if( gnrl_query.lastError().isValid() )
        qDebug() << gnrl_query.lastError();

    while( gnrl_query.next() )
    {
        QSqlRecord record = gnrl_query.record();
        p->general[record.value(0).toString()] = record.value(1).toString();
    }


    QSqlQuery syncs_query(p->db);
    syncs_query.prepare("SELECT id,revision,last_revision FROM Sync");
    syncs_query.exec();

    if( syncs_query.lastError().isValid() )
        qDebug() << syncs_query.lastError();

    while( syncs_query.next() )
    {
        QSqlRecord record = syncs_query.record();
        SyncItem item;
        item.id = record.value(0).toString();
        item.revision = record.value(1).toInt();
        item.last_revision = record.value(1).toInt();

        p->syncs[item.id] = item;
    }


    QSqlQuery syncs_pprs_query(p->db);
    syncs_pprs_query.prepare("SELECT uuid FROM Papers");
    syncs_pprs_query.exec();

    if( syncs_pprs_query.lastError().isValid() )
        qDebug() << syncs_pprs_query.lastError();

    while( syncs_pprs_query.next() )
    {
        QSqlRecord record = syncs_pprs_query.record();
        const QString & uuid = record.value(0).toString();
        if( p->syncs.contains(uuid) )
            continue;

        SyncItem item;
        item.id = uuid;
        item.revision = -1;
        item.last_revision = -1;

        p->syncs[item.id] = item;
    }

    if( !p->syncs.contains(GROUPS_SYNC_KEY) )
        p->syncs[GROUPS_SYNC_KEY].id = GROUPS_SYNC_KEY;

    QSqlQuery ppr_query(p->db);
    ppr_query.prepare("SELECT max(id) FROM Papers");
    ppr_query.exec();

    if( ppr_query.lastError().isValid() )
        qDebug() << ppr_query.lastError();

    if( ppr_query.next() )
    {
        QSqlRecord record = ppr_query.record();
        p->max_paper_id = record.value(0).toInt()+1;
    }
}

void Database::update_database()
{
    BEGIN_FNC_DEBUG
    const QString & version = value("version");
    if( version.isEmpty() )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "ALTER TABLE Groups RENAME TO tmp_Groups;";
        query_list << "CREATE  TABLE IF NOT EXISTS Groups (id INT NOT NULL ,name TEXT ,color VARCHAR(9) NOT NULL ,uuid VARCHAR(64) NOT NULL ,PRIMARY KEY (id) );";
        query_list << "INSERT INTO Groups(id,name,color,uuid) SELECT id,name,color,uuid FROM tmp_Groups;";
        query_list << "DROP TABLE tmp_Groups;";
        query_list << "CREATE UNIQUE INDEX uuid_g_UNIQUE ON Groups (uuid ASC);";
        query_list << "COMMIT;";

        foreach( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","1");
    }
    if( value("version") == "1" )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "ALTER TABLE Papers ADD COLUMN latitude REAL DEFAULT 0;";
        query_list << "ALTER TABLE Papers ADD COLUMN longitude REAL DEFAULT 0;";
        query_list << "ALTER TABLE Papers ADD COLUMN altitude REAL DEFAULT 0;";
        query_list << "CREATE INDEX latitude_p_UNIQUE ON Papers (latitude ASC);";
        query_list << "CREATE INDEX longitude_p_UNIQUE ON Papers (longitude ASC);";
        query_list << "CREATE INDEX altitude_p_UNIQUE ON Papers (altitude ASC);";
        query_list << "COMMIT;";

        foreach( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","2");
    }
    if( value("version") == "2" )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "ALTER TABLE Papers ADD COLUMN type INT DEFAULT 0;";
        query_list << "CREATE INDEX type_p_UNIQUE ON Papers (type ASC);";
        query_list << "COMMIT;";

        foreach( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","3");
    }
    if( value("version") == "3" )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "DROP INDEX IF EXISTS latitude_p_UNIQUE;";
        query_list << "DROP INDEX IF EXISTS longitude_p_UNIQUE;";
        query_list << "DROP INDEX IF EXISTS altitude_p_UNIQUE;";
        query_list << "DROP INDEX IF EXISTS type_p_UNIQUE;";
        query_list << "CREATE INDEX type_p ON Papers (type ASC);";
        query_list << "CREATE INDEX latitude_p ON Papers (latitude ASC);";
        query_list << "CREATE INDEX longitude_p ON Papers (longitude ASC);";
        query_list << "CREATE INDEX altitude_p ON Papers (altitude ASC);";
        query_list << "COMMIT;";

        foreach( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","4");
    }
    if( value("version") == "4" )
    {
        setRevision(GROUPS_SYNC_KEY,-1);
        setValue("version","5");
    }
    if( value("version") == "5" )
    {
        QStringList query_list;
        query_list << "BEGIN;";
        query_list << "ALTER TABLE Papers ADD COLUMN weather VARCHAR(64) DEFAULT NULL;";
        query_list << "ALTER TABLE Papers ADD COLUMN temperature INT DEFAULT NULL;";
        query_list << "CREATE INDEX weather_p ON Papers (weather ASC);";
        query_list << "CREATE INDEX temperature_p ON Papers (temperature ASC);";
        query_list << "COMMIT;";

        foreach( const QString & query_str, query_list )
            QSqlQuery( query_str, p->db ).exec();

        setValue("version","6");
    }
    END_FNC_DEBUG
}

PaperClass Database::getPaper(int id)
{
    if( p->cached_papers_ids.contains(id) )
    {
        foreach( const PaperClass & paper, p->papers )
            if( paper.id == id )
                return paper;

        p->cached_papers_ids.remove(id);
    }

    QSqlQuery query(p->db);
    query.prepare("SELECT id,uuid,title,text,ctime,cdate,mtime,mdate,activity,grp,latitude,longitude,altitude,type,weather,temperature FROM Papers WHERE id=:id");
    query.bindValue(":id",id);
    query.exec();

    if( query.lastError().isValid() )
        qDebug() << query.lastError();

    PaperClass result;
    result.id = -1;

    if( !query.next() )
        return result;

    QSqlRecord record = query.record();
    result.id       = record.value(0).toInt();
    result.uuid     = record.value(1).toString();
    result.title    = record.value(2).toString();
    result.text     = record.value(3).toString();
    result.create   = QDateTime( QDate(1,1,1).addDays(record.value(5).toInt()), QTime(0,0,0).addSecs(record.value(4).toInt()) );
    result.modified = QDateTime( QDate(1,1,1).addDays(record.value(7).toInt()), QTime(0,0,0).addSecs(record.value(6).toInt()) );
    result.activity = record.value(8).toInt();
    result.group    = record.value(9).toInt();
    result.location = QGeoCoordinate( record.value(10).toDouble(), record.value(11).toDouble(), record.value(12).toDouble() );
    result.type     = static_cast<Enums::PaperType>(record.value(13).toInt());
    result.weather     = record.value(14).toString();
    result.temperature = record.value(15).toInt();

    p->papers << result;
    p->cached_papers_ids.insert(result.id);

    while( p->papers.count() > CACHE_SIZE )
    {
        const PaperClass & paper = p->papers.takeFirst();
        p->cached_papers_ids.remove(paper.id);
    }

    return result;
}

void Database::savePaper(PaperClass paper)
{
    bool allow_exec = true;
    if( paper.title.isEmpty() && paper.text.isEmpty() && paper.modified.isNull() )
    {
        const QStringList & files = paperFiles(paper.id);
        if( files.isEmpty() )
            allow_exec = false;
    }

    BEGIN
    paper.modified = QDateTime::currentDateTime();

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Papers (id,uuid,title,text,ctime,cdate,mtime,mdate,activity,grp,latitude,longitude,altitude,type,weather,temperature) "
                  "VALUES (:id,:uuid,:title,:text,:ctime,:cdate,:mtime,:mdate,:actvt,:grp,:ltud,:lgud,:atud,:type,:wthr,:tmp)");
    query.bindValue(":id"    ,paper.id);
    query.bindValue(":uuid"  ,paper.uuid);
    query.bindValue(":title" ,paper.title);
    query.bindValue(":text"  ,paper.text);
    query.bindValue(":ctime" ,QTime(0,0,0).secsTo(paper.create.time()));
    query.bindValue(":cdate" ,QDate(1,1,1).daysTo(paper.create.date()));
    query.bindValue(":mtime" ,QTime(0,0,0).secsTo(paper.modified.time()));
    query.bindValue(":mdate" ,QDate(1,1,1).daysTo(paper.modified.date()));
    query.bindValue(":actvt" ,paper.activity);
    query.bindValue(":grp"   ,paper.group);
    query.bindValue(":ltud"  ,paper.location.latitude());
    query.bindValue(":lgud"  ,paper.location.longitude());
    query.bindValue(":atud"  ,paper.location.altitude());
    query.bindValue(":type"  ,static_cast<int>(paper.type));
    query.bindValue(":tmp"   ,paper.temperature);
    query.bindValue(":wthr"  ,paper.weather);

    if( allow_exec )
        query.exec();

    if( p->cached_papers_ids.contains(paper.id) )
    {
        for( int i=0; i<p->papers.count(); i++ )
        {
            PaperClass & ppr = p->papers[i];
            if( ppr.id == paper.id )
                ppr = paper;

        }
    }
    else
    {
        p->papers << paper;
        p->cached_papers_ids.insert(paper.id);
    }

    while( p->papers.count() > CACHE_SIZE )
    {
        const PaperClass & paper = p->papers.takeFirst();
        p->cached_papers_ids.remove(paper.id);
    }

    if( allow_exec )
        setRevision(paper.uuid,-1);

    DB_EMIT paperChanged(paper.id);
    COMMIT
}

void Database::saveGroup(GroupClass group)
{
    BEGIN
    if( group.uuid.isEmpty() )
    {
        if( group.id == 0 )
            group.uuid = NO_GROUP_UUID;
        else
            group.uuid = QUuid::createUuid().toString();
    }

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Groups (id,name,color,uuid) VALUES (:id,:name,:color,:uuid)");
    query.bindValue(":id"   , group.id);
    query.bindValue(":name" , group.name);
    query.bindValue(":color", group.color.name());
    query.bindValue(":uuid" , group.uuid);
    query.exec();

    p->groups[group.id] = group;

    setRevision( GROUPS_SYNC_KEY, -1 );
    emit groupsListChanged();
    COMMIT
}

void Database::saveActivity(ActivityClass activity)
{
    BEGIN

    QSqlQuery query(p->db);
    query.prepare("INSERT OR REPLACE INTO Activity (id,name,description,image) VALUES (:id,:name,:dsc,:img)");
    query.bindValue(":id"  , activity.id);
    query.bindValue(":name", activity.name);
    query.bindValue(":dsc" , activity.description);
    query.bindValue(":img" , activity.image);
    query.exec();

    DB_EMIT activityListChanged();
    COMMIT
}

void Database::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == p->commit_timer )
    {
        killTimer(p->commit_timer);
        p->commit_timer = 0;

        commit();
    }
}

Database::~Database()
{
    delete p;
}

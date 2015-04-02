#include "papyrushistorymodel.h"
#include "papyrus.h"
#include "database.h"
#include "asemantools/asemancalendarconverter.h"

class PapyrusHistoryModelPrivate
{
public:
    Database *db;
    QList<int> list;
};

PapyrusHistoryModel::PapyrusHistoryModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PapyrusHistoryModelPrivate;
    p->db = Papyrus::database();

    connect(p->db, SIGNAL(datesListChanged()), SLOT(refresh()));
    refresh();
}

int PapyrusHistoryModel::id(const QModelIndex &index) const
{
    const int row = index.row();
    return p->list.at(row);
}

int PapyrusHistoryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant PapyrusHistoryModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const int did = id(index);
    switch(role)
    {
    case HistoryDate:
        result = did;
        break;

    case HistoryText:
    {
        AsemanCalendarConverter *calendarConv = Papyrus::instance()->calendarConverter();
        const int currentDate = calendarConv->currentDays();
        if( did == currentDate )
            result = tr("Today");
        else
        if( did == currentDate-1 )
            result = tr("Yesterday");
        else
        if( did == currentDate-2 )
            result = tr("Two days ago");
        else
            result = calendarConv->convertIntToFullStringDate(did);
    }
        break;

    case HistoryPapersCount:
    {
        AsemanCalendarConverter *calendarConv = Papyrus::instance()->calendarConverter();
        result = p->db->papersCountOf(calendarConv->convertDaysToDate(did));
    }
        break;
    }

    return result;
}

QHash<qint32, QByteArray> PapyrusHistoryModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( HistoryDate, "date");
    res->insert( HistoryText, "text");
    res->insert( HistoryPapersCount, "papersCount");

    return *res;
}

int PapyrusHistoryModel::count() const
{
    return p->list.count();
}

void PapyrusHistoryModel::refresh()
{
    changed(p->db->dates());
}

void PapyrusHistoryModel::changed(const QList<int> &list)
{
    bool count_changed = (list.count()==p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const int did = p->list.at(i);
        if( list.contains(did) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<int> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const int did = temp_list.at(i);
        if( p->list.contains(did) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const int did = p->list.at(i);
            int nw = temp_list.indexOf(did);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const int did = list.at(i);
        if( p->list.contains(did) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, did );
        endInsertRows();
    }

    if(count_changed)
        emit countChanged();

    emit datesChanged();
}

PapyrusHistoryModel::~PapyrusHistoryModel()
{
    delete p;
}


#include "papyruslabelsmodel.h"
#include "database.h"
#include "papyrus.h"

class PapyrusLabelsModelPrivate
{
public:
    QList<int> list;
    Database *db;
};

PapyrusLabelsModel::PapyrusLabelsModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PapyrusLabelsModelPrivate;
    p->db = Papyrus::database();

    connect(p->db, SIGNAL(groupsListChanged()), SLOT(refresh()) );

    connect(p->db, SIGNAL(groupNameChanged(int)), SLOT(groupNameChanged(int)));
    connect(p->db, SIGNAL(groupColorChanged(int)), SLOT(groupColorChanged(int)));
    connect(p->db, SIGNAL(groupPapersCountChanged(int)), SLOT(groupPapersCountChanged(int)));

    refresh();
}

int PapyrusLabelsModel::id(const QModelIndex &index) const
{
    const int row = index.row();
    return p->list.at(row);
}

int PapyrusLabelsModel::indexOf(int id)
{
    return p->list.indexOf(id);
}

int PapyrusLabelsModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant PapyrusLabelsModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const int gid = id(index);
    switch(role)
    {
    case GroupId:
        result = gid;
        break;

    case GroupName:
        result = p->db->groupName(gid);
        break;

    case GroupColor:
        result = p->db->groupColor(gid);
        break;

    case GroupUuid:
        result = p->db->groupUuid(gid);
        break;

    case GroupPapersCount:
        result = p->db->groupPapersCount(gid);
        break;
    }

    return result;
}

QHash<qint32, QByteArray> PapyrusLabelsModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( GroupId, "id");
    res->insert( GroupName, "name");
    res->insert( GroupColor, "color");
    res->insert( GroupUuid, "uuid");
    res->insert( GroupPapersCount, "papersCount");

    return *res;
}

int PapyrusLabelsModel::count() const
{
    return p->list.count();
}

void PapyrusLabelsModel::refresh()
{
    changed(p->db->groups());
}

void PapyrusLabelsModel::groupNameChanged(int id)
{
    roleDataChanged(id, GroupName);
}

void PapyrusLabelsModel::groupColorChanged(int id)
{
    roleDataChanged(id, GroupColor);
}

void PapyrusLabelsModel::groupPapersCountChanged(int id)
{
    roleDataChanged(id, GroupPapersCount);
}

void PapyrusLabelsModel::roleDataChanged(int id, int role)
{
    const int idx = indexOf(id);
    if(idx == -1)
        return;

    emit dataChanged(index(idx), index(idx), QVector<int>()<<role);
}

void PapyrusLabelsModel::changed(QList<int> list)
{
    for(int i=0; i<list.count(); i++)
        if(p->db->groupIsDeleted(list.at(i)))
        {
            list.removeAt(i);
            i--;
        }

    bool count_changed = (list.count()==p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const int gid = p->list.at(i);
        if( list.contains(gid) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<int> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const int gid = temp_list.at(i);
        if( p->list.contains(gid) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const int gid = p->list.at(i);
            int nw = temp_list.indexOf(gid);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const int gid = list.at(i);
        if( p->list.contains(gid) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, gid );
        endInsertRows();
    }

    if(count_changed)
        emit countChanged();

    emit groupsChanged();
}

PapyrusLabelsModel::~PapyrusLabelsModel()
{
    delete p;
}


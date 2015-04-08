#include "papyrussidebarmodel.h"
#include "database.h"
#include "papyrus.h"
#include "asemantools/asemancalendarconverter.h"

#include <QUrl>
#include <QTimer>
#include <QDebug>

class PapyrusSideBarModelAction
{
public:
    PapyrusSideBarModelAction() : dateId(0), groupId(0) {}

    PapyrusSideBarModel::PapyrusSideBarModelTypes type;
    PapyrusSideBarModel::PapyrusSideBarModelSections section;
    int dateId;
    int groupId;

    QUrl actionIcon;
    QString actionName;
    QVariant actionCommand;

    bool operator==(const PapyrusSideBarModelAction &b) {
        return  type == b.type &&
                section == b.section &&
                dateId == b.dateId &&
                groupId == b.groupId &&
                actionIcon == b.actionIcon &&
                actionName == b.actionName &&
                actionCommand == b.actionCommand;
    }
};

class PapyrusSideBarModelPrivate
{
public:
    QList<PapyrusSideBarModelAction> list;
    QList<PapyrusSideBarModelAction> actions;
    Database *db;
    QTimer *refresh_timer;
};

PapyrusSideBarModel::PapyrusSideBarModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PapyrusSideBarModelPrivate;
    p->db = Papyrus::database();

    p->refresh_timer = new QTimer(this);
    p->refresh_timer->setInterval(100);
    p->refresh_timer->setSingleShot(true);

    connect(p->refresh_timer, SIGNAL(timeout()), SLOT(refresh()));

    connect(p->db, SIGNAL(datesListChanged()) , SLOT(update()));
    connect(p->db, SIGNAL(groupsListChanged()), SLOT(update()));

    connect(p->db, SIGNAL(groupNameChanged(int)), SLOT(groupNameChanged(int)));
    connect(p->db, SIGNAL(groupColorChanged(int)), SLOT(groupColorChanged(int)));
    connect(p->db, SIGNAL(groupPapersCountChanged(int)), SLOT(groupPapersCountChanged(int)));
    connect(p->db, SIGNAL(paperCreated(int)), SLOT(paperCreated(int)), Qt::QueuedConnection);
    connect(p->db, SIGNAL(paperDeleted(int)), SLOT(paperCreated(int)), Qt::QueuedConnection);

    refresh();
}

int PapyrusSideBarModel::id(const QModelIndex &index) const
{
    return index.row();
}

int PapyrusSideBarModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant PapyrusSideBarModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const int idx = id(index);
    const PapyrusSideBarModelAction &action = p->list.at(idx);

    switch(role)
    {
    case SideBarType:
        result = action.type;
        break;

    case SideBarSection:
        result = action.section;
        break;

    case SideBarHistoryDate:
        result = action.dateId;
        break;

    case SideBarGroupId:
        result = action.groupId;
        break;

    case SideBarGroupUuid:
        result = p->db->groupUuid(action.groupId);
        break;

    case SideBarItemPapersCount:
        if(action.type == TypeHistory)
        {
            const int did = action.dateId;
            AsemanCalendarConverter *calendarConv = Papyrus::instance()->calendarConverter();
            result = p->db->papersCountOf(calendarConv->convertDaysToDate(did));
        }
        else
            result = p->db->groupPapersCount(action.groupId);
        break;

    case SideBarActionCommand:
        result = action.actionCommand;
        break;

    case SideBarItemName:
        if(action.type == TypeHistory)
        {
            const int did = action.dateId;
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
        else
        if(action.type == TypeGroup)
            result = p->db->groupName(action.groupId);
        else
        if(action.type == TypeAction)
            result = action.actionName;
        else
            result = QString();
        break;

    case SideBarItemColor:
        if(action.type == TypeGroup)
            result = p->db->groupColor(action.groupId);
        else
            result = QColor();
        break;

    case SideBarItemIcon:
        if(action.type == TypeHistory)
            result = QUrl();
        else
        if(action.type == TypeGroup)
            result = QUrl();
        else
        if(action.type == TypeAction)
            result = action.actionIcon;
        else
            result = QUrl();
        break;
    }

    return result;
}

QHash<qint32, QByteArray> PapyrusSideBarModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( SideBarType, "type");
    res->insert( SideBarSection, "section");
    res->insert( SideBarGroupId, "groupId");
    res->insert( SideBarGroupUuid, "uuid");
    res->insert( SideBarItemPapersCount, "papersCount");
    res->insert( SideBarHistoryDate, "date");
    res->insert( SideBarItemName, "name");
    res->insert( SideBarItemColor, "color");
    res->insert( SideBarItemIcon, "icon");
    res->insert( SideBarActionCommand, "command");

    return *res;
}

int PapyrusSideBarModel::count() const
{
    return p->list.count();
}

int PapyrusSideBarModel::indexOf(int type, int id)
{
    switch(type)
    {
    case TypeGroup:
        for(int i=0; i<p->list.count(); i++)
        {
            const PapyrusSideBarModelAction &action = p->list.at(i);
            if(action.type == type && action.groupId == id)
                return i;
        }
        return -1;
        break;

    case TypeHistory:
        for(int i=0; i<p->list.count(); i++)
        {
            const PapyrusSideBarModelAction &action = p->list.at(i);
            if(action.type == type && action.dateId == id)
                return i;
        }
        return -1;
        break;
    }

    return -1;
}

void PapyrusSideBarModel::refresh()
{
    QList<PapyrusSideBarModelAction> result;

    QList<PapyrusSideBarModelAction> actions;
    QList<PapyrusSideBarModelAction> history;
    QList<PapyrusSideBarModelAction> groups;

    foreach(const PapyrusSideBarModelAction &action, p->actions)
    {
        switch(static_cast<int>(action.section))
        {
        case SectionAction:
            actions << action;
            break;

        case SectionGroup:
            groups << action;
            break;

        case SectionHistory:
            history << action;
            break;
        }
    }

    const QList<int> &dates = p->db->lastThreeDays();
    foreach(int date, dates)
    {
        PapyrusSideBarModelAction action;
        action.dateId = date;
        action.section = SectionHistory;
        action.type = TypeHistory;

        history << action;
    }

    const QList<int> &labels = p->db->groups();
    foreach(int label, labels)
    {
        if(p->db->groupIsDeleted(label))
            continue;

        PapyrusSideBarModelAction action;
        action.groupId = label;
        action.section = SectionGroup;
        action.type = TypeGroup;

        groups << action;
    }

    result << actions;
    result << history;
    result << groups;

    changed(result);
}

void PapyrusSideBarModel::update()
{
    p->refresh_timer->stop();
    p->refresh_timer->start();
}

void PapyrusSideBarModel::addAction(int section, const QString &name, const QUrl &icon, const QVariant &command)
{
    PapyrusSideBarModelAction action;
    action.type = TypeAction;
    action.section = static_cast<PapyrusSideBarModelSections>(section);
    action.actionName = name;
    action.actionIcon = icon;
    action.actionCommand = command;

    p->actions << action;
    update();
}

void PapyrusSideBarModel::clearActions()
{
    p->actions.clear();
    update();
}

void PapyrusSideBarModel::groupNameChanged(int id)
{
    roleDataChanged(id, SideBarItemName);
}

void PapyrusSideBarModel::groupColorChanged(int id)
{
    roleDataChanged(id, SideBarItemColor);
}

void PapyrusSideBarModel::groupPapersCountChanged(int id)
{
    roleDataChanged(id, SideBarItemPapersCount);
}

void PapyrusSideBarModel::paperCreated(int id)
{
    const QDateTime &dt = p->db->paperCreatedDate(id);
    const int date = QDate(1,1,1).daysTo(dt.date());

    const int idx = indexOf(TypeHistory, date);
    if(idx == -1)
        return;

    emit dataChanged(index(idx), index(idx), QVector<int>()<<SideBarItemPapersCount);
}

void PapyrusSideBarModel::roleDataChanged(int id, int role)
{
    const int idx = indexOf(TypeGroup, id);
    if(idx == -1)
        return;

    emit dataChanged(index(idx), index(idx), QVector<int>()<<role);
}

void PapyrusSideBarModel::changed(const QList<PapyrusSideBarModelAction> &list)
{
    bool count_changed = (list.count()==p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const PapyrusSideBarModelAction &action = p->list.at(i);
        if( list.contains(action) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<PapyrusSideBarModelAction> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const PapyrusSideBarModelAction &action = temp_list.at(i);
        if( p->list.contains(action) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const PapyrusSideBarModelAction &action = p->list.at(i);
            int nw = temp_list.indexOf(action);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const PapyrusSideBarModelAction &action = list.at(i);
        if( p->list.contains(action) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, action );
        endInsertRows();
    }

    if(count_changed)
        emit countChanged();

    emit listChanged();
}

PapyrusSideBarModel::~PapyrusSideBarModel()
{
    delete p;
}


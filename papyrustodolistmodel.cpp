#include "papyrustodolistmodel.h"

#include <QDebug>

class PapyrusToDoListItem
{
public:
    PapyrusToDoListItem() : type(PapyrusToDoListModel::TypeTextOnly), checked(false){}
    QString text;
    PapyrusToDoListModel::ToDoListItemType type;
    bool checked;

    bool operator ==(const PapyrusToDoListItem &b) {
        return text == b.text &&
               checked == b.checked &&
               type == b.type;
    }
};

class PapyrusToDoListModelPrivate
{
public:
    QString text;
    QList<PapyrusToDoListItem> list;
};

PapyrusToDoListModel::PapyrusToDoListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PapyrusToDoListModelPrivate;
    p->list << PapyrusToDoListItem();
}

int PapyrusToDoListModel::id(const QModelIndex &index) const
{
    const int row = index.row();
    return row;
}

int PapyrusToDoListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant PapyrusToDoListModel::data(const QModelIndex &index, int role) const
{
    const int idx = id(index);
    const PapyrusToDoListItem &item = p->list.at(idx);
    QVariant result;

    switch(role)
    {
    case ToDoListText:
        result = item.text;
        break;

    case ToDoListType:
        result = item.type;
        break;

    case ToDoListChecked:
        result = item.checked;
        break;
    }

    return result;
}

bool PapyrusToDoListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    const int idx = id(index);
    PapyrusToDoListItem &item = p->list[idx];
    bool result = true;

    switch(role)
    {
    case ToDoListText:
        item.text = value.toString();
        break;

    case ToDoListType:
        item.type = static_cast<ToDoListItemType>(value.toInt());
        break;

    case ToDoListChecked:
        item.checked = value.toBool();
        break;

    default:
        result = false;
    }

    p->text = generateText();
    emit textChanged();

    if(result)
        emit dataChanged(index, index, QVector<int>()<<role);

    return result;
}

QHash<qint32, QByteArray> PapyrusToDoListModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( ToDoListText, "text");
    res->insert( ToDoListType, "type");
    res->insert( ToDoListChecked, "checked");

    return *res;
}

Qt::ItemFlags PapyrusToDoListModel::flags(const QModelIndex &index) const
{
    Q_UNUSED(index)
    return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

int PapyrusToDoListModel::count() const
{
    return p->list.count();
}

QString PapyrusToDoListModel::text() const
{
    return p->text;
}

void PapyrusToDoListModel::setText(const QString &txt)
{
    if(p->text == txt)
        return;

    p->text = txt;

    QList<PapyrusToDoListItem> list;
    int lastPos = 0;
    int pos = txt.indexOf("\n",lastPos);

    while( pos != txt.length() )
    {
        if( pos == -1 )
            pos = txt.length();

        QString str = txt.mid(lastPos,pos-lastPos);
        QString slc = str.mid(0,2);
        bool checkable = (slc=="* " || slc=="- ");
        if( !checkable && !list.isEmpty() )
        {
            PapyrusToDoListItem &item = list.last();
            item.text = QString(item.text + "\n" + str).trimmed();
        }
        else
        {
            PapyrusToDoListItem item;
            item.type = checkable? TypeCheckable : TypeTextOnly;
            item.text = checkable? str.mid(2) : str;
            item.checked = (slc=="* "? true : false);
            list << item;
        }

        lastPos = pos+1;
        if( pos != txt.length() )
            pos = txt.indexOf("\n",lastPos);
    }

    changed(list);
    emit textChanged();
}

void PapyrusToDoListModel::insertLine(int idx, const QString &text)
{
    PapyrusToDoListItem item;
    item.type = TypeCheckable;
    item.text = text;
    item.checked = false;

    QList<PapyrusToDoListItem> list = p->list;
    list.insert(idx, item);

    changed(list);

    p->text = generateText();
    emit textChanged();
}

void PapyrusToDoListModel::deleteLine(int idx)
{
    QList<PapyrusToDoListItem> list = p->list;
    list.removeAt(idx);

    changed(list);

    p->text = generateText();
    emit textChanged();
}

void PapyrusToDoListModel::changed(const QList<PapyrusToDoListItem> &list)
{
    bool count_changed = (list.count()==p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const PapyrusToDoListItem & item = p->list.at(i);
        if( list.contains(item) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QList<PapyrusToDoListItem> temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const PapyrusToDoListItem & item = temp_list.at(i);
        if( p->list.contains(item) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const PapyrusToDoListItem & item = p->list.at(i);
            int nw = temp_list.indexOf(item);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const PapyrusToDoListItem & item = list.at(i);
        if( p->list.contains(item) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, item );
        endInsertRows();
    }

    if(count_changed)
        emit countChanged();

    emit listChanged();
}

QString PapyrusToDoListModel::generateText() const
{
    QString result;
    for( int i=0; i<p->list.count(); i++ )
    {
        const PapyrusToDoListItem &item = p->list.at(i);
        if( item.type == TypeCheckable )
            result += item.checked? "* " : "- ";

        result += item.text;
        if( i < p->list.count()-1 )
            result += "\n";
    }

    return result;
}

PapyrusToDoListModel::~PapyrusToDoListModel()
{
    delete p;
}


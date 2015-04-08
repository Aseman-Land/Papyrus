#include "papyruspaperfilesmodel.h"
#include "database.h"
#include "papyrus.h"
#include "repository.h"

#include <QFileInfo>

class PapyrusPaperFilesModelPrivate
{
public:
    int paperId;
    QStringList list;
    Database *db;
};

PapyrusPaperFilesModel::PapyrusPaperFilesModel(QObject *parent) :
    QAbstractListModel(parent)
{
    p = new PapyrusPaperFilesModelPrivate;
    p->paperId = -1;
    p->db = Papyrus::database();

    connect(p->db, SIGNAL(filesListChanged()), SLOT(refresh()));
}

void PapyrusPaperFilesModel::setPaperId(int id)
{
    if(p->paperId == id)
        return;

    p->paperId = id;
    emit paperIdChanged();

    refresh();
}

int PapyrusPaperFilesModel::paperId() const
{
    return p->paperId;
}

QString PapyrusPaperFilesModel::id(const QModelIndex &index) const
{
    const int row = index.row();
    return p->list.at(row);
}

int PapyrusPaperFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return count();
}

QVariant PapyrusPaperFilesModel::data(const QModelIndex &index, int role) const
{
    QVariant result;
    const QString &fileId = id(index);
    switch(role)
    {
    case RoleFileId:
        result = fileId;
        break;

    case RoleFilePath:
        result = Papyrus::instance()->repository()->getPath(fileId);
        break;

    case RoleFileSuffix:
        result = QFileInfo(fileId).suffix().toLower();
        break;
    }

    return result;
}

QHash<qint32, QByteArray> PapyrusPaperFilesModel::roleNames() const
{
    static QHash<qint32, QByteArray> *res = 0;
    if( res )
        return *res;

    res = new QHash<qint32, QByteArray>();
    res->insert( RoleFileId, "id");
    res->insert( RoleFilePath, "path");
    res->insert( RoleFileSuffix, "suffix");

    return *res;
}

int PapyrusPaperFilesModel::count() const
{
    return p->list.count();
}

void PapyrusPaperFilesModel::refresh()
{
    if(p->paperId == -1)
        changed(QStringList());
    else
        changed( p->db->paperFiles(p->paperId) );
}

void PapyrusPaperFilesModel::changed(const QStringList &list)
{
    bool count_changed = (list.count()==p->list.count());

    for( int i=0 ; i<p->list.count() ; i++ )
    {
        const QString &fileId = p->list.at(i);
        if( list.contains(fileId) )
            continue;

        beginRemoveRows(QModelIndex(), i, i);
        p->list.removeAt(i);
        i--;
        endRemoveRows();
    }

    QStringList temp_list = list;
    for( int i=0 ; i<temp_list.count() ; i++ )
    {
        const QString &fileId = temp_list.at(i);
        if( p->list.contains(fileId) )
            continue;

        temp_list.removeAt(i);
        i--;
    }
    while( p->list != temp_list )
        for( int i=0 ; i<p->list.count() ; i++ )
        {
            const QString &fileId = p->list.at(i);
            int nw = temp_list.indexOf(fileId);
            if( i == nw )
                continue;

            beginMoveRows( QModelIndex(), i, i, QModelIndex(), nw>i?nw+1:nw );
            p->list.move( i, nw );
            endMoveRows();
        }

    for( int i=0 ; i<list.count() ; i++ )
    {
        const QString &fileId = list.at(i);
        if( p->list.contains(fileId) )
            continue;

        beginInsertRows(QModelIndex(), i, i );
        p->list.insert( i, fileId );
        endInsertRows();
    }

    if(count_changed)
        emit countChanged();

    emit listChanged();
}

PapyrusPaperFilesModel::~PapyrusPaperFilesModel()
{
    delete p;
}


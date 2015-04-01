#ifndef PAPYRUSLABELSMODEL_H
#define PAPYRUSLABELSMODEL_H

#include <QAbstractListModel>

class PapyrusLabelsModelPrivate;
class PapyrusLabelsModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(LabelsModelRoles)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum LabelsModelRoles {
        GroupId = Qt::UserRole,
        GroupName,
        GroupColor,
        GroupUuid,
        GroupPapersCount
    };

    PapyrusLabelsModel(QObject *parent = 0);
    ~PapyrusLabelsModel();

    int id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;
    int count() const;

public slots:
    void refresh();

signals:
    void countChanged();
    void groupsChanged();

private:
    void changed(const QList<int> &list);

private:
    PapyrusLabelsModelPrivate *p;
};

#endif // PAPYRUSLABELSMODEL_H

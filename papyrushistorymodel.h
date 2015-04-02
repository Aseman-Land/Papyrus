#ifndef PAPYRUSHISTORYMODEL_H
#define PAPYRUSHISTORYMODEL_H

#include <QAbstractListModel>

class PapyrusHistoryModelPrivate;
class PapyrusHistoryModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(HistoryModelRoles)

    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum HistoryModelRoles {
        HistoryDate = Qt::UserRole,
        HistoryText,
        HistoryPapersCount
    };

    PapyrusHistoryModel(QObject *parent = 0);
    ~PapyrusHistoryModel();

    int id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;
    int count() const;

public slots:
    void refresh();

signals:
    void countChanged();
    void datesChanged();

private:
    void changed(const QList<int> &list);

private:
    PapyrusHistoryModelPrivate *p;
};

#endif // PAPYRUSHISTORYMODEL_H

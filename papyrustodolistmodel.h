#ifndef PAPYRUSTODOLISTMODEL_H
#define PAPYRUSTODOLISTMODEL_H

#include <QAbstractListModel>

class PapyrusToDoListItem;
class PapyrusToDoListModelPrivate;
class PapyrusToDoListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(ToDoListModelRoles)
    Q_ENUMS(ToDoListItemType)

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    Q_PROPERTY(QString text READ text NOTIFY textChanged)

public:
    enum ToDoListModelRoles {
        ToDoListText = Qt::UserRole,
        ToDoListType,
        ToDoListChecked
    };

    enum ToDoListItemType {
        TypeTextOnly,
        TypeCheckable
    };

    PapyrusToDoListModel(QObject *parent = 0);
    ~PapyrusToDoListModel();

    int id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role);

    QHash<qint32,QByteArray> roleNames() const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    int count() const;

    QString text() const;

public slots:
    void setText(const QString &txt);

signals:
    void countChanged();
    void listChanged();
    void textChanged();

private:
    void changed(const QList<PapyrusToDoListItem> &list);
    QString generateText() const;

private:
    PapyrusToDoListModelPrivate *p;
};

#endif // PAPYRUSTODOLISTMODEL_H

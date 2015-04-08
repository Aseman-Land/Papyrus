#ifndef PAPYRUSSIDEBARMODEL_H
#define PAPYRUSSIDEBARMODEL_H

#include <QAbstractListModel>

class PapyrusSideBarModelAction;
class PapyrusSideBarModelPrivate;
class PapyrusSideBarModel : public QAbstractListModel
{
    Q_OBJECT
    Q_ENUMS(PapyrusSideBarModelRoles)
    Q_ENUMS(PapyrusSideBarModelTypes)
    Q_ENUMS(PapyrusSideBarModelSections)

public:
    enum PapyrusSideBarModelRoles {
        SideBarType = Qt::UserRole,
        SideBarSection,

        SideBarHistoryDate,

        SideBarGroupId,
        SideBarGroupUuid,

        SideBarActionCommand,

        SideBarItemName,
        SideBarItemIcon,
        SideBarItemColor,
        SideBarItemPapersCount,
    };

    enum PapyrusSideBarModelTypes {
        TypeHistory,
        TypeGroup,
        TypeAction
    };

    enum PapyrusSideBarModelSections {
        SectionHistory,
        SectionGroup,
        SectionAction
    };

    PapyrusSideBarModel(QObject *parent = 0);
    ~PapyrusSideBarModel();

    int id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;
    int count() const;

    int indexOf(int type, int id);

public slots:
    void refresh();
    void update();

    void addAction(int section, const QString &name, const QUrl &icon, const QVariant &command);
    void clearActions();

signals:
    void countChanged();
    void listChanged();

private slots:
    void groupNameChanged(int id);
    void groupColorChanged(int id);
    void groupPapersCountChanged(int id);
    void paperCreated(int id);

private:
    void roleDataChanged(int id, int role);
    void changed(const QList<PapyrusSideBarModelAction> &list);

private:
    PapyrusSideBarModelPrivate *p;
};

#endif // PAPYRUSSIDEBARMODEL_H

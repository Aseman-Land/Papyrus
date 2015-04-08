#ifndef PAPYRUSPAPERFILESMODEL_H
#define PAPYRUSPAPERFILESMODEL_H

#include <QAbstractListModel>
#include <QStringList>

class PapyrusPaperFilesModelPrivate;
class PapyrusPaperFilesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int paperId READ paperId WRITE setPaperId NOTIFY paperIdChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum PaperFilesModelRoles {
        RoleFileId = Qt::UserRole,
        RoleFilePath,
        RoleFileSuffix
    };

    PapyrusPaperFilesModel(QObject *parent = 0);
    ~PapyrusPaperFilesModel();

    void setPaperId(int id);
    int paperId() const;

    QString id( const QModelIndex &index ) const;

    int rowCount(const QModelIndex & parent = QModelIndex()) const;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

    QHash<qint32,QByteArray> roleNames() const;
    int count() const;

public slots:
    void refresh();

signals:
    void countChanged();
    void listChanged();
    void paperIdChanged();

private:
    void changed(const QStringList &list);

private:
    PapyrusPaperFilesModelPrivate *p;
};

#endif // PAPYRUSPAPERFILESMODEL_H

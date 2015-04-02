#ifndef PAPYRUSHISTORYMODEL_H
#define PAPYRUSHISTORYMODEL_H

#include <QObject>

class PapyrusHistoryModel : public QObject
{
    Q_OBJECT
public:
    explicit PapyrusHistoryModel(QObject *parent = 0);
    ~PapyrusHistoryModel();

signals:

public slots:
};

#endif // PAPYRUSHISTORYMODEL_H

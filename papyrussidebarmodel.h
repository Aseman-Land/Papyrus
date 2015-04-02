#ifndef PAPYRUSSIDEBARMODEL_H
#define PAPYRUSSIDEBARMODEL_H

#include <QObject>

class PapyrusSideBarModel : public QObject
{
    Q_OBJECT
public:
    explicit PapyrusSideBarModel(QObject *parent = 0);
    ~PapyrusSideBarModel();

signals:

public slots:
};

#endif // PAPYRUSSIDEBARMODEL_H

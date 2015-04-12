#ifndef PAPYRUSABSTRACTPAPER_H
#define PAPYRUSABSTRACTPAPER_H

#include <QQuickItem>
#include "papercoreobject.h"

class PapyrusAbstractPaperPrivate;
class PapyrusAbstractPaper : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(PaperCoreObject* paperCore READ paperCore WRITE setPaperCore NOTIFY paperCoreChanged)
    Q_PROPERTY(bool interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)

public:
    PapyrusAbstractPaper(QQuickItem *parent = 0);
    ~PapyrusAbstractPaper();

    void setPaperCore(PaperCoreObject *core);
    PaperCoreObject *paperCore() const;

    void setInteractive(bool stt);
    bool interactive() const;

signals:
    void paperCoreChanged();
    void interactiveChanged();

private:
    PapyrusAbstractPaperPrivate *p;
};

#endif // PAPYRUSABSTRACTPAPER_H

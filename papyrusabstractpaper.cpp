#include "papyrusabstractpaper.h"

#include <QPointer>

class PapyrusAbstractPaperPrivate
{
public:
    QPointer<PaperCoreObject> core;
    bool interactive;
};

PapyrusAbstractPaper::PapyrusAbstractPaper(QQuickItem *parent) :
    QQuickItem(parent)
{
    p = new PapyrusAbstractPaperPrivate;
    p->interactive = true;
}

void PapyrusAbstractPaper::setPaperCore(PaperCoreObject *core)
{
    if(p->core == core)
        return;

    p->core = core;
    emit paperCoreChanged();
}

PaperCoreObject *PapyrusAbstractPaper::paperCore() const
{
    return p->core;
}

void PapyrusAbstractPaper::setInteractive(bool stt)
{
    if(p->interactive == stt)
        return;

    p->interactive = stt;
    emit interactiveChanged();
}

bool PapyrusAbstractPaper::interactive() const
{
    return p->interactive;
}

PapyrusAbstractPaper::~PapyrusAbstractPaper()
{
    delete p;
}


#ifndef PAPYRUSPAPERDRAGAREA_H
#define PAPYRUSPAPERDRAGAREA_H

#include <QQuickItem>

class PapyrusPaperDragAreaPrivate;
class PapyrusPaperDragArea : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(int orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)
    Q_PROPERTY(int minimum READ minimum WRITE setMinimum NOTIFY minimumChanged)
    Q_PROPERTY(int mouseX READ mouseX NOTIFY mouseXChanged)
    Q_PROPERTY(int mouseY READ mouseY NOTIFY mouseYChanged)

public:
    PapyrusPaperDragArea(QQuickItem *parent = 0);
    ~PapyrusPaperDragArea();

    void setMinimum(int min);
    int minimum() const;

    void setOrientation(int ori);
    int orientation() const;

    int mouseX() const;
    int mouseY() const;

signals:
    void minimumChanged();
    void orientationChanged();

    void positionChanged();
    void mouseXChanged();
    void mouseYChanged();
    void pressed();
    void released();

protected:
    bool childMouseEventFilter(QQuickItem *item, QEvent *event);

private:
    PapyrusPaperDragAreaPrivate *p;
};

#endif // PAPYRUSPAPERDRAGAREA_H

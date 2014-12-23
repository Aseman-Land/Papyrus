/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    Papyrus is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Papyrus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef PAPERMANAGER_H
#define PAPERMANAGER_H

#include <QQuickItem>
#include <QStringList>
#include <QGeoCoordinate>

class Papyrus;
class PaperManagerPrivate;
class PaperManager : public QQuickItem
{
    Q_PROPERTY(int root READ root WRITE setRoot NOTIFY rootChanged)
    Q_PROPERTY(int type READ type WRITE setType NOTIFY typeChanged)
    Q_PROPERTY(QString keyword READ keyword WRITE setKeyword NOTIFY keywordChanged)
    Q_PROPERTY(QQuickItem *currentPaper READ currentPaper NOTIFY currentPaperChanged)
    Q_PROPERTY(QList<int> papers READ papers NOTIFY papersChanged)
    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_ENUMS(Type)

    Q_OBJECT
public:
    enum Type {
        Group,
        Date,
        Search,
        AdvanceSearch,
        Clean,
        All
    };

    PaperManager(QQuickItem *parent = 0);
    ~PaperManager();

    void setRoot( int id );
    int root() const;

    void setType( int t );
    int type() const;

    void setKeyword( const QString & kw );
    QString keyword() const;

    QQuickItem *currentPaper() const;

    QList<int> papers() const;

    void setCurrentIndex( int idx );
    int currentIndex();

public slots:
    void setAdvanceSearch( const QString & keyword, const QDate & startDate, const QDate & endDate, const QTime & startTime, const QTime & endTime, int group, int domain, const QRectF & geo, const QString & weather );

signals:
    void rootChanged();
    void typeChanged();
    void keywordChanged();
    void currentPaperChanged();
    void papersChanged();
    void currentIndexChanged();

private slots:
    QQuickItem *createPaper();
    Papyrus *getPapyrus();

    void init_buffer();
    void reindexBuffer();
    void load_buffers();

    void paperClosed(const QVariant &var);
    void paperEntered(const QVariant &var);
    void paperSaved(int id);

    void deletePaper(const QVariant &var );

    void outMove(QVariant size );
    void outMoveFinished();

private:
    PaperManagerPrivate *p;
};

#endif // PAPERMANAGER_H

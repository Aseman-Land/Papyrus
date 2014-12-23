/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "qmlmapcontrol.h"
#include <QTimer>
#include <QtMath>
#include <QTouchEvent>
#include <QDebug>

namespace qmapcontrol
{

    QmlMapControl::QmlMapControl (QQuickItem *parent) :
        QQuickPaintedItem(parent)
    {
        setOpaquePainting(true);
        setAcceptHoverEvents(true);
        setAcceptedMouseButtons(Qt::AllButtons);

        layermanager = new LayerManager(this, size);
        screen_middle = QPoint(size.width()/2, size.height()/2);

        mousepressed = false;
        scaleVisible = true;
        crosshairsVisible = true;
        mymousemode = Panning;
        mouse_wheel_events = true;

        connect(ImageManager::instance(), SIGNAL(imageReceived()),
                this, SLOT(updateRequestNew()));

        connect(ImageManager::instance(), SIGNAL(loadingFinished()),
                this, SLOT(loadingFinished()));

        setWidth(size.width()+1);
        setHeight(size.height()+1);

        connect( this, SIGNAL(widthChanged()) , SLOT(sizeChanged()) );
        connect( this, SIGNAL(heightChanged()), SLOT(sizeChanged()) );
    }

    QmlMapControl::~QmlMapControl()
    {
        delete layermanager;
    }

    void QmlMapControl::enableMouseWheelEvents( bool enabled )
    {
        mouse_wheel_events = enabled;
    }

    bool QmlMapControl::mouseWheelEventsEnabled()
    {
        return mouse_wheel_events;
    }

    QPointF QmlMapControl::currentCoordinate() const
    {
        return layermanager->currentCoordinate();
    }

    Layer* QmlMapControl::layer(const QString& layername) const
    {
        return layermanager->layer(layername);
    }

    QList<QString> QmlMapControl::layers() const
    {
        return layermanager->layers();
    }

    int QmlMapControl::numberOfLayers() const
    {
        return layermanager->layers().size();
    }

    void QmlMapControl::followGeometry(const Geometry* geom) const
    {
        if ( geom == 0 )
        {
            return;
        }

        //ensures only one signal is ever connected
        stopFollowing(geom);

        connect(geom, SIGNAL(positionChanged(Geometry*)),
                this, SLOT(positionChanged(Geometry*)));
    }

    void QmlMapControl::positionChanged(Geometry* geom)
    {
        if ( !layermanager->layer() || !layermanager->layer()->mapAdapter() )
        {
            qDebug() << "MapControl::positionChanged() - no layers configured";
            return;
        }

        QPoint start = layermanager->layer()->mapAdapter()->coordinateToDisplay(currentCoordinate());
        QPoint dest = layermanager->layer()->mapAdapter()->coordinateToDisplay(((Point*)geom)->coordinate());

        QPoint step = (dest-start);

        layermanager->scrollView(step);

        updateRequestNew();
    }

    void QmlMapControl::sizeChanged()
    {
        QSize newSize = QSize(width(),height());
        if( newSize == size )
            return;

        resize( newSize );
    }

    void QmlMapControl::moveTo(QPointF coordinate)
    {
        target = coordinate;
        steps = 25;
        if (moveMutex.tryLock())
        {
            QTimer::singleShot(40, this, SLOT(tick()));
        }
        else
        {
            // stopMove(coordinate);
            moveMutex.unlock();
        }
    }
    void QmlMapControl::tick()
    {
        if ( !layermanager->layer() || !layermanager->layer()->mapAdapter() )
        {
            qDebug() << "MapControl::tick() - no layers configured";
            return;
        }

        QPoint start = layermanager->layer()->mapAdapter()->coordinateToDisplay(currentCoordinate());
        QPoint dest = layermanager->layer()->mapAdapter()->coordinateToDisplay(target);

        QPoint step = (dest-start)/steps;
        layermanager->scrollView(step);

        update();
        layermanager->updateRequest();
        steps--;
        if (steps>0)
        {
            QTimer::singleShot(40, this, SLOT(tick()));
        }
        else
        {
            moveMutex.unlock();
        }
    }

    void QmlMapControl::paint(QPainter* painter)
    {
        // painter->translate(150,190);
        // painter->scale(0.5,0.5);

        // painter->setClipRect(0,0, size.width(), size.height());

        // painter->setViewport(10000000000,0,size.width(),size.height());

        /*
        // rotating
            rotation = 45;
            painter->translate(256,256);
            painter->rotate(rotation);
            painter->translate(-256,-256);
        */

        layermanager->drawImage(painter);
        layermanager->drawGeoms(painter);

        // added by wolf
        // draw scale
        if (scaleVisible)
        {
            QList<double> distanceList;
            distanceList<<5000000<<2000000<<1000000<<1000000<<1000000<<100000<<100000<<50000<<50000<<10000<<10000<<10000<<1000<<1000<<500<<200<<100<<50<<25;
            if (currentZoom() >= 0 && distanceList.size() > currentZoom())
            {
                double line;
                line = distanceList.at( currentZoom() ) / pow(2.0, 18-currentZoom() ) / 0.597164;

                // draw the scale
                painter->setPen(Qt::black);
                QPoint p1(10,size.height()-20);
                QPoint p2((int)line,size.height()-20);
                painter->drawLine(p1,p2);

                painter->drawLine(10,size.height()-15, 10,size.height()-25);
                painter->drawLine((int)line,size.height()-15, (int)line,size.height()-25);

                QString distance;
                if (distanceList.at(currentZoom()) >= 1000)
                {
                    distance = QVariant( distanceList.at(currentZoom())/1000 )  .toString()+ " km";
                }
                else
                {
                    distance = QVariant( distanceList.at(currentZoom()) ).toString() + " m";
                }

                painter->drawText(QPoint((int)line+10,size.height()-15), distance);
            }
        }

        if (crosshairsVisible)
        {
            painter->drawLine(screen_middle.x(), screen_middle.y()-10,
                             screen_middle.x(), screen_middle.y()+10); // |
            painter->drawLine(screen_middle.x()-10, screen_middle.y(),
                             screen_middle.x()+10, screen_middle.y()); // -
        }

        // int cross_x = int(layermanager->getMapmiddle_px().x())%256;
        // int cross_y = int(layermanager->getMapmiddle_px().y())%256;
        // painter->drawLine(screen_middle.x()-cross_x+cross_x, screen_middle.y()-cross_y+0,
        //   screen_middle.x()-cross_x+cross_x, screen_middle.y()-cross_y+256); // |
        // painter->drawLine(screen_middle.x()-cross_x+0, screen_middle.y()-cross_y+cross_y,
        //   screen_middle.x()-cross_x+256, screen_middle.y()-cross_y+cross_y); // -

        painter->drawRect(0,0, size.width(), size.height());
        /*
        // rotating
          painter->setMatrix(painter->matrix().inverted());
        //qt = painter->transform();
           qm = painter->combinedMatrix();
        */

        if (mousepressed && mymousemode == Dragging)
        {
            QRect rect = QRect(pre_click_px, current_mouse_pos);
            painter->drawRect(rect);
        }
        //emit viewChanged(currentCoordinate(), currentZoom());
    }

    // mouse events
    void QmlMapControl::mousePressEvent(QMouseEvent* evnt)
    {
        //rotating (experimental)
        // QMouseEvent* me = new QMouseEvent(evnt->type(), qm.map(QPoint(evnt->x(),evnt->y())), evnt->button(), evnt->buttons(), evnt->modifiers());
        // evnt = me;
        // qDebug() << "evnt: " << evnt->x() << ", " << evnt->y() << ", " << evnt->pos();

        layermanager->mouseEvent(evnt);

        if (layermanager->layers().size()>0)
        {
            if (evnt->button() == 1)
            {
                mousepressed = true;
                pre_click_px = QPoint(evnt->x(), evnt->y());
            }
            else if ( evnt->button() == 2  &&
                      mouseWheelEventsEnabled() &&
                      mymousemode != None) // zoom in
            {
                zoomIn();
            }
            else if  ( evnt->button() == 4 &&
                         mouseWheelEventsEnabled() &&
                         mymousemode != None) // zoom out
            {
                zoomOut();
            }
        }

        // emit(mouseEvent(evnt));
        emit(mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())));
        evnt->accept();
    }

    void QmlMapControl::mouseReleaseEvent(QMouseEvent* evnt)
    {
        mousepressed = false;
        if (mymousemode == Dragging)
        {
            QPointF ulCoord = clickToWorldCoordinate(pre_click_px);
            QPointF lrCoord = clickToWorldCoordinate(current_mouse_pos);

            QRectF coordinateBB = QRectF(ulCoord, QSizeF( (lrCoord-ulCoord).x(), (lrCoord-ulCoord).y()));

            emit(boxDragged(coordinateBB));
        }

        emit(mouseEventCoordinate(evnt, clickToWorldCoordinate(evnt->pos())));
        evnt->accept();
    }

    void QmlMapControl::mouseMoveEvent(QMouseEvent* evnt)
    {
        // emit(mouseEvent(evnt));

        /*
        // rotating
           QMouseEvent* me = new QMouseEvent(evnt->type(), qm.map(QPoint(evnt->x(),evnt->y())), evnt->button(), evnt->buttons(), evnt->modifiers());
           evnt = me;
        */
        if (mousepressed && mymousemode == Panning)
        {
            QPoint offset = pre_click_px - QPoint(evnt->x(), evnt->y());
            layermanager->scrollView(offset);
            pre_click_px = QPoint(evnt->x(), evnt->y());
        }
        else if (mousepressed && mymousemode == Dragging)
        {
            current_mouse_pos = QPoint(evnt->x(), evnt->y());
        }

        evnt->accept();
        update();
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::wheelEvent(QWheelEvent *evnt)
    {
        if(mouse_wheel_events &&
            evnt->orientation() == Qt::Vertical)
        {
            if(evnt->delta() > 0)
            {
                if( currentZoom() == 17 )
                {
                    return;
                }

                QPoint pnt = evnt->pos();
                QPoint newPoint( width()-pnt.x(), height()-pnt.y() );

                setView(clickToWorldCoordinate(pnt));
                zoomIn(pnt);
                setView(clickToWorldCoordinate(newPoint));
            }
            else
            {
                if( currentZoom() == 0 )
                {
                    return;
                }
                QPoint pnt = evnt->pos();
                QPoint newPoint( width()-pnt.x(), height()-pnt.y() );

                setView(clickToWorldCoordinate(pnt));
                zoomOut(pnt);
                setView(clickToWorldCoordinate(newPoint));
            }
            evnt->accept();
        }
        else
        {
            evnt->ignore();
        }
    }

    void QmlMapControl::touchEvent(QTouchEvent *evnt)
    {
        const QList<QTouchEvent::TouchPoint> & touchs = evnt->touchPoints();
        if( touchs.count() != 2 )
        {
            QQuickPaintedItem::touchEvent(evnt);
        }
        else
        {
            evnt->accept();

            const QTouchEvent::TouchPoint & t0 = touchs.first();
            const QTouchEvent::TouchPoint & t1 = touchs.last();

            if( last_t0_startPos.isNull() )
                last_t0_startPos = t0.startPos();
            if( last_t1_startPos.isNull() )
                last_t1_startPos = t1.startPos();

            qreal startW = qPow( qPow(last_t0_startPos.x()-last_t1_startPos.x(),2)+qPow(last_t0_startPos.y()-last_t1_startPos.y(),2), 0.5 );
            qreal endW = qPow( qPow(t0.pos().x()-t1.pos().x(),2)+qPow(t0.pos().y()-t1.pos().y(),2), 0.5 );

            if( startW*4/3<endW )
            {
                QPoint pnt( last_t0_startPos.x()/2+last_t1_startPos.x()/2, last_t0_startPos.y()/2+last_t1_startPos.y()/2 );
                QPoint newPoint( width()-pnt.x(), height()-pnt.y() );

                setView(clickToWorldCoordinate(pnt));
                zoomIn(pnt);
                setView(clickToWorldCoordinate(newPoint));

                last_t0_startPos = t0.pos();
                last_t1_startPos = t1.pos();
            }
            else
            if( startW*3/4>endW )
            {
                QPoint pnt( t0.pos().x()/2+t1.pos().x()/2, t0.pos().y()/2+t1.pos().y()/2 );
                QPoint newPoint( width()-pnt.x(), height()-pnt.y() );

                setView(clickToWorldCoordinate(pnt));
                zoomOut(pnt);
                setView(clickToWorldCoordinate(newPoint));

                last_t0_startPos = t0.pos();
                last_t1_startPos = t1.pos();
            }
        }
    }

    void QmlMapControl::refresh()
    {
        update();
    }

    QPointF QmlMapControl::clickToWorldCoordinate(QPoint click)
    {
        if ( !layermanager->layer() || !layermanager->layer()->mapAdapter() )
        {
            qDebug() << "MapControl::clickToWorldCoordinate() - no layers configured";
            return QPointF();
        }
        // click coordinate to image coordinate
        QPoint displayToImage= QPoint(click.x()-screen_middle.x()+layermanager->getMapmiddle_px().x(),
                                      click.y()-screen_middle.y()+layermanager->getMapmiddle_px().y());

        // image coordinate to world coordinate
        return layermanager->layer()->mapAdapter()->displayToCoordinate(displayToImage);
    }

    void QmlMapControl::updateRequest(QRect rect)
    {
        update(rect);
    }

    void QmlMapControl::updateRequestNew()
    {
        layermanager->forceRedraw();
    }

    // slots
    void QmlMapControl::zoomIn(QPoint middle)
    {
        if( currentZoom() == 17 )
        {
            return;
        }

        layermanager->zoomIn(middle);
        updateView();
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::zoomOut(QPoint middle)
    {
        if( currentZoom() == 0 )
        {
            return;
        }

        layermanager->zoomOut(middle);
        updateView();
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::setZoom(int zoomlevel)
    {
        if ( currentZoom() == zoomlevel ||
             zoomlevel < 0 ||
             zoomlevel > 17 )
        {
            return;
        }

        layermanager->setZoom(zoomlevel);
        updateView();
        emit viewChanged(currentCoordinate(), currentZoom());
        refresh();
    }

    int QmlMapControl::currentZoom() const
    {
        return layermanager->currentZoom();
    }

    void QmlMapControl::scrollLeft(int pixel)
    {
        layermanager->scrollView(QPoint(-pixel,0));
        updateView();
    }

    void QmlMapControl::scrollRight(int pixel)
    {
        layermanager->scrollView(QPoint(pixel,0));
        updateView();
    }

    void QmlMapControl::scrollUp(int pixel)
    {
        layermanager->scrollView(QPoint(0,-pixel));
        updateView();
    }

    void QmlMapControl::scrollDown(int pixel)
    {
        layermanager->scrollView(QPoint(0,pixel));
        updateView();
    }

    void QmlMapControl::scroll(const QPoint scroll)
    {
        layermanager->scrollView(scroll);
        updateView();
    }

    void QmlMapControl::updateView() const
    {
        layermanager->setView( currentCoordinate() );
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::setView(const QPointF& coordinate) const
    {
        layermanager->setView(coordinate);
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::setView(const QList<QPointF> coordinates) const
    {
        layermanager->setView(coordinates);
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::setViewAndZoomIn(const QList<QPointF> coordinates) const
    {
        layermanager->setViewAndZoomIn(coordinates);
        emit viewChanged(currentCoordinate(), currentZoom());
    }

    void QmlMapControl::setView(const Point* point)
    {
        layermanager->setView(point->coordinate());
        refresh();
    }

    void QmlMapControl::loadingFinished()
    {
        // qDebug() << "MapControl::loadingFinished()";
        layermanager->removeZoomImage();
    }
    void QmlMapControl::addLayer(Layer* layer)
    {
        layermanager->addLayer(layer);
        update();
    }

    void QmlMapControl::addLayer(QObject *layer)
    {
        addLayer( static_cast<Layer*>(layer) );
    }

    void QmlMapControl::removeLayer( Layer* layer )
    {
        disconnect(layer, 0, 0, 0);
        layermanager->removeLayer( layer );
        update();
    }

    void QmlMapControl::setMouseMode(MouseMode mousemode)
    {
        if( mymousemode == mousemode )
            return;

        mymousemode = mousemode;
        emit mouseModeChanged();
    }

    QmlMapControl::MouseMode QmlMapControl::mouseMode()
    {
        return mymousemode;
    }

    void QmlMapControl::stopFollowing(const Geometry* geom) const
    {
        disconnect(geom,SIGNAL(positionChanged(Geometry*)), this, SLOT(positionChanged(Geometry*)));
    }

    void QmlMapControl::enablePersistentCache( int tileExpiry, const QDir& path)
    {
        ImageManager::instance()->setCacheDir(tileExpiry, path);
    }

    void QmlMapControl::setProxy(QString host, int port, const QString username, const QString password)
    {
        ImageManager::instance()->setProxy(host, port, username, password);
    }

    void QmlMapControl::showScale(bool visible)
    {
        if( scaleVisible == visible )
            return;

        scaleVisible = visible;
        emit scaleVisibleChanged();
    }

    bool QmlMapControl::scaleIsVisible() const
    {
        return scaleVisible;
    }

    void QmlMapControl::showCrosshairs(bool visible)
    {
        if( crosshairsVisible == visible )
            return;

        crosshairsVisible = visible;
        emit crosshairsVisibleChanged();
    }

    bool QmlMapControl::crosshairsIsVisible() const
    {
        return crosshairsVisible;
    }

    void QmlMapControl::resize(const QSize newSize)
    {
        this->size = newSize;
        screen_middle = QPoint(newSize.width()/2, newSize.height()/2);

        setWidth(newSize.width());
        setHeight(newSize.height());
        layermanager->resize(newSize);

        emit viewChanged(currentCoordinate(), currentZoom());
    }

   void QmlMapControl::setUseBoundingBox( bool usebounds )
   {
        if( layermanager )
            layermanager->setUseBoundingBox( usebounds );
   }

   bool QmlMapControl::isBoundingBoxEnabled()
   {
        if( layermanager )
            return layermanager->isBoundingBoxEnabled();
        return false;
   }

   void QmlMapControl::setBoundingBox( QRectF &rect )
   {
        if( layermanager )
            layermanager->setBoundingBox( rect );
   }

   QRectF QmlMapControl::getBoundingBox()
   {
        if( layermanager )
            return layermanager->getBoundingBox();

        // Return an empty QRectF if there is no layermanager
        return QRectF();
   }

   QRectF QmlMapControl::getViewport()
   {
       if( layermanager )
           return layermanager->getViewport();

       // Return an empty QRectF if there is no layermanager
       return QRectF();
   }

   bool QmlMapControl::isGeometryVisible( Geometry * geometry)
   {
       if ( !geometry || getViewport() == QRectF() )
           return false;

       return getViewport().contains( geometry->boundingBox() );
   }
}

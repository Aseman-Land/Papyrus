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

import QtQuick 2.0
import Papyrus 1.0

Item {
    id: msw
    width: 100
    height: 62

    property rect viewPort
    property point view
    property int zoom
    property bool scaleHelper
    property bool crosshairs

    property variant item

    onViewChanged: if(item) item.view = view
    onZoomChanged: if(item) item.zoom = zoom
    onScaleHelperChanged: if(item) item.scaleHelper = scaleHelper
    onCrosshairsChanged: if(item) item.crosshairs = crosshairs

    Component.onCompleted: {
        if( papyrus.mapMode == Papyrus.GoogleMap ) {
            item = google_map_compoennt.createObject(msw)
        } else if( papyrus.mapMode == Papyrus.OpenStreetMap ) {
            item = os_map_component.createObject(msw)
        }
    }

    Component {
        id: google_map_compoennt
        GoogleMapView {
            anchors.fill: parent

            onViewPortChanged: msw.viewPort = viewPort
            onViewChanged: msw.view = view
            onZoomChanged: msw.zoom = zoom
            onScaleHelperChanged: msw.scaleHelper = scaleHelper
            onCrosshairsChanged: msw.crosshairs = crosshairs

            Component.onCompleted: {
                view = msw.view
                zoom = msw.zoom
                scaleHelper = msw.scaleHelper
                crosshairs = msw.crosshairs
            }
        }
    }

    Component {
        id: os_map_component
        OSMapView {
            anchors.fill: parent

            onViewPortChanged: msw.viewPort = viewPort
            onViewChanged: msw.view = view
            onZoomChanged: msw.zoom = zoom
            onScaleHelperChanged: msw.scaleHelper = scaleHelper
            onCrosshairsChanged: msw.crosshairs = crosshairs

            Component.onCompleted: {
                view = msw.view
                zoom = msw.zoom
                scaleHelper = msw.scaleHelper
                crosshairs = msw.crosshairs
            }
        }
    }
}

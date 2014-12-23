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
    width: 100
    height: 62
    clip: true

    property alias viewPort: map_control.viewPort
    property alias view: map_control.view
    property alias zoom: map_control.zoom
    property alias scaleHelper: map_control.scaleHelper
    property alias crosshairs: map_control.crosshairs

    OSMMapAdapter {
        id: osm_adapter
    }

    MapLayer {
        id: map_layer
        mapAdapter: osm_adapter.adapter
        layerName: "OpenStreetMap-Layer"
    }

    MapControl {
        id: map_control
        anchors.fill: parent
        anchors.margins: -4*Devices.density
        scaleHelper: true
        crosshairs: false
        visible: true
        Component.onCompleted: {
            var geo = position.coordinate
            addLayer(map_layer.layerObject)
            view = Qt.point(geo.longitude,geo.latitude)
            zoom = 15
        }
    }
}

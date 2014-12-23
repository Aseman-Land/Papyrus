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
import QtPositioning 5.2
import AsemanTools 1.0

Rectangle {
    id: map_view
    width: 100
    height: 62

    property real longitude
    property real latitude

    property bool unknown: !latitude && !longitude
    property int paperId

    Image {
        id: splash_map
        source: "files/map-loading.png"
        anchors.fill: parent
        sourceSize: Qt.size(width,height)
        visible: !master_map.visible
        fillMode: Image.PreserveAspectCrop
        asynchronous: true

        Text {
            anchors.left: parent.left
            anchors.top: parent.top
            font.family: AsemanApp.globalFontFamily
            font.pixelSize: 9*Devices.fontDensity
            text: latitude + ", " + longitude
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            font.family: AsemanApp.globalFontFamily
            font.pixelSize: 13*Devices.fontDensity
            text: unknown? qsTr("Unknown point") : qsTr("Loading Map...")
        }
    }

    Image {
        id: master_map
        anchors.fill: parent
        asynchronous: true
        visible: master_map.status == Image.Ready && !unknown
        sourceSize: Qt.size(width,height)
        source: "http://maps.google.com/maps/api/staticmap?center=" + latitude + "," + longitude + "&zoom=15&size=" + Math.floor(width) + "x" + Math.floor(height) + "&sensor=false"
    }

    Image {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.verticalCenter
        source: "files/map-pin.png"
        sourceSize: Qt.size(width,height)
        fillMode: Image.PreserveAspectFit
        width: 92*Devices.density
        height: 92*Devices.density
        visible: master_map.visible
        asynchronous: true
        smooth: true
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        visible: !unknown
        onClicked: {
            if( !longitude && !latitude )
                Qt.openUrlExternally("http://maps.google.com")
            else
                Qt.openUrlExternally("http://maps.google.com/maps?&q=" + latitude + "," + longitude)
        }
    }
}

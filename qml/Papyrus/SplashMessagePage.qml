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
import AsemanTools 1.0

Item {
    id: page
    width: 100
    height: 62

    property alias text: txt.text
    property alias image: img.source

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10*Devices.density

        Text {
            id: txt
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: 18*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            horizontalAlignment: Text.AlignHCenter
            wrapMode: Text.WrapAtWordBoundaryOrAnywhere
            color: "#333333"
        }

        Image {
            id: img
            anchors.horizontalCenter: parent.horizontalCenter
            width: parent.width*0.7
            height: width*1.47
            sourceSize: Qt.size(width,height)
            fillMode: Image.PreserveAspectFit
        }
    }
}

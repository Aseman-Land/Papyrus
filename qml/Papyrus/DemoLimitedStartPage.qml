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
    anchors.fill: parent

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter

        Text {
            id: limit_text
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 30*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
            height: 70*Devices.density
            text: qsTr("TRIAL EXPIRED")
        }

        Text {
            id: choice_text
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 13*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
            text: qsTr("Now you have 2 choice")
        }
    }

    Text {
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: scroll_img.top
        font.pixelSize: 9*Devices.fontDensity
        font.family: AsemanApp.globalFontFamily
        color: "#666666"
        text: qsTr("scroll down")
    }

    Image {
        id: scroll_img
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 20*Devices.density
        width: 80*Devices.density
        height: width
        sourceSize: Qt.size(width,height)
        fillMode: Image.PreserveAspectFit
        source: "files/scroll-down.png"
    }
}

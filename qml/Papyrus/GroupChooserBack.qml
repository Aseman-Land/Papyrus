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
    id: group_chooser_back
    width: Devices.isMobile? 75*Devices.density : 100*Devices.density
    height: 30*Devices.density

    property real radius: 3*Devices.density
    property color color: "#aaaaaa"

    property int group
    property bool press: false

    onGroupChanged: {
        txt.text = database.groupName(group)
        color = database.groupColor(group)
    }

    Rectangle{
        anchors.fill: parent
        anchors.bottomMargin: group_chooser_back.radius
        color: group_chooser_back.press? "#3B97EC" : group_chooser_back.color
    }

    Rectangle{
        anchors.fill: parent
        radius: group_chooser_back.radius
        color: group_chooser_back.press? "#3B97EC" : group_chooser_back.color
    }

    Text{
        id: txt
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        font.bold: Devices.isTablet
        font.pixelSize: Devices.isTouchDevice? (Devices.isMobile? 8*Devices.fontDensity : 9*Devices.fontDensity) : 10*Devices.fontDensity
        font.family: AsemanApp.globalFont.family
        color: "#ffffff"
        horizontalAlignment: Text.AlignHCenter
        maximumLineCount: 1
        elide: Text.ElideRight
        wrapMode: Text.WrapAnywhere
        text: qsTr("No Label")
    }

}

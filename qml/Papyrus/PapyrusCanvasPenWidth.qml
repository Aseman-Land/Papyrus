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

ListView {
    id: color_list
    anchors.fill: parent
    anchors.margins: 15*Devices.density
    clip: true
    model: ListModel{}

    property real selectedPen: 1

    delegate: Rectangle {
        anchors.right: parent.right
        anchors.left: parent.left
        height: 48*Devices.density
        color: mousearea.pressed? "#aa0d80ec" : "#00000000"

        Item {
            id: item
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.margins: 4*Devices.density
            width: height

            Rectangle{
                id: rectangle
                anchors.centerIn: parent
                width: (itemWidth+2)*Devices.density
                height: width
                radius: width/2
                color: "#000000"
            }
        }

        Text {
            x: 40*Devices.density
            anchors.verticalCenter: parent.verticalCenter
            text: itemWidth + " " + qsTr("Pixel")
            horizontalAlignment: Text.AlignLeft
            font.family: AsemanApp.globalFontFamily
            font.pixelSize: 11*Devices.fontDensity
            font.bold: selectedPen == itemWidth
            color: "#333333"
        }

        MouseArea{
            id: mousearea
            anchors.fill: parent
            hoverEnabled: true
            onClicked: {
                selectedPen = itemWidth
                hidePointDialog()
            }
        }
    }

    ScrollBar {
        scrollArea: color_list; height: color_list.height; width: 6*Devices.density
        anchors.right: color_list.right; anchors.top: color_list.top; color: "#000000"
    }

    Component.onCompleted: {
        model.clear()
        for( var i=1; i<11; i++ )
            model.append({"itemWidth": i})
    }
}

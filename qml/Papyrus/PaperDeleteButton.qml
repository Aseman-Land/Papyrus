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

PaperAbstractButton {
    property variant dateChooser

    onClicked: {
        if( !dateChooser )
            dateChooser = showBottomPanel(delete_component,true)
    }

    Rectangle {
        anchors.fill: parent
        color: "#ff5532"
        radius: 4*Devices.density
        anchors.margins: 5*Devices.density

        Text {
            anchors.centerIn: parent
            font.pixelSize: 12*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#ffffff"
            text: qsTr("Delete Paper")
        }
    }

    Component {
        id: delete_component
        Column {
            id: paper_types
            height: 100*Devices.density

            MenuButton {
                height: 50*Devices.density
                width: paper_types.width
                normalColor: "#00000000"
                highlightColor: "#ff5532"
                textColor: press? "#ffffff" : "#ff5532"
                textFont.weight: Font.Normal
                textFont.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Delete It")
                onClicked: {
                    paperObject.deleteRequest()
                    hideBottomPanel()
                    hideSubMessage()
                }
            }

            MenuButton {
                height: 50*Devices.density
                width: paper_types.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Cancel")
                onClicked: {
                    hideBottomPanel()
                }
            }
        }
    }
}

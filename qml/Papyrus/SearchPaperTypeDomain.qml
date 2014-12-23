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

import QtQuick 2.0
import Papyrus 1.0
import AsemanTools 1.0

Item {
    id: ptype_domain
    width: 100
    height: 62

    property int domain: Enums.AllPapers
    property string selectedText: "All Papers"

    Column {
        id: column
        anchors.fill: parent

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }

        MenuButton {
            height: 50*Devices.density
            width: column.width
            normalColor: "#00000000"
            highlightColor: "#4098bf"
            textColor: press? "#ffffff" : "#4098bf"
            textFont.weight: Font.Normal
            textFont.pixelSize: 13*Devices.fontDensity
            textFont.bold: false
            text: qsTr("Normal Papers")
            onClicked: {
                selectedText = text
                ptype_domain.domain = Enums.NormalPapers
            }
        }

        MenuButton {
            height: 50*Devices.density
            width: column.width
            normalColor: "#00000000"
            highlightColor: "#4098bf"
            textColor: press? "#ffffff" : "#4098bf"
            textFont.weight: Font.Normal
            textFont.pixelSize: 13*Devices.fontDensity
            textFont.bold: false
            text: qsTr("To-Do Papers")
            onClicked: {
                selectedText = text
                ptype_domain.domain = Enums.ToDoPapers
            }
        }

        MenuButton {
            height: 50*Devices.density
            width: column.width
            normalColor: "#00000000"
            highlightColor: "#4098bf"
            textColor: press? "#ffffff" : "#4098bf"
            textFont.weight: Font.Normal
            textFont.pixelSize: 13*Devices.fontDensity
            textFont.bold: false
            text: qsTr("Completed Tasks")
            onClicked: {
                selectedText = text
                ptype_domain.domain = Enums.CompletedTasks
            }
        }

        MenuButton {
            height: 50*Devices.density
            width: column.width
            normalColor: "#00000000"
            highlightColor: "#4098bf"
            textColor: press? "#ffffff" : "#4098bf"
            textFont.weight: Font.Normal
            textFont.pixelSize: 13*Devices.fontDensity
            textFont.bold: false
            text: qsTr("Uncompleted Tasks")
            onClicked: {
                selectedText = text
                ptype_domain.domain = Enums.UncompletedTasks
            }
        }
    }
}

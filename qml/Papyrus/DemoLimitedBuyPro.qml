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
    id: buy_pro
    anchors.fill: parent

    signal buy()

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 40*Devices.density
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10*Devices.density

        Text {
            id: limit_text
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: 30*Devices.fontDensity
            font.family: AsemanApp.globalFont.family
            height: 70*Devices.density
            horizontalAlignment: Text.AlignHCenter
            color: "#333333"
            text: qsTr("BUY PAPYRUS")
        }

        Text {
            id: choice_text
            anchors.left: parent.left
            anchors.right: parent.right
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            font.pixelSize: 12*Devices.fontDensity
            font.family: AsemanApp.globalFont.family
            color: "#333333"
            text: qsTr("Buy papyrus unlimited version")
        }
    }

    Button {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 40*Devices.density
        anchors.rightMargin: 40*Devices.density
        anchors.bottomMargin: 20*Devices.density
        height: 42*Devices.density
        normalColor: "#4098bf"
        highlightColor: "#337fa2"
        text: qsTr("Buy")
        onClicked: buy_pro.buy()
    }
}

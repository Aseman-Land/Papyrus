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

import QtQuick 2.2
import AsemanTools 1.0

Item {
    id: about_page

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Column {
        spacing: 2
        anchors.verticalCenter: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter

        Image{
            id: papyrus_logo
            anchors.horizontalCenter: parent.horizontalCenter
            width: 192*Devices.density
            height: width
            smooth: true
            source: "files/papyrus.png"
        }

        Text{
            id: about_name_text
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Devices.isMobile? 13*Devices.fontDensity : 15*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
        }

        Text{
            id: about_license_text
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"

            MouseArea{
                anchors.fill: parent
                onClicked: Qt.openUrlExternally("http://www.gnu.org/licenses/gpl-3.0.en.html")
            }
        }

        Text{
            id: about_powered_text
            anchors.left: parent.left
            anchors.right: parent.right
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"

            MouseArea{
                anchors.fill: parent
                onClicked: Qt.openUrlExternally("http://aseman.co/")
            }
        }
    }

    Button {
        id: about_home
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.leftMargin: 40*Devices.density
        anchors.rightMargin: 40*Devices.density
        anchors.bottomMargin: 20*Devices.density
        height: 42*Devices.density
        normalColor: "#4098bf"
        highlightColor: "#337fa2"
        onClicked: Qt.openUrlExternally("http://papyrus.org/")
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        about_name_text.text      = qsTr("Papyrus") + " " + papyrus.version()
        about_license_text.text   = qsTr("License: GNU GPL Version 3")
        about_powered_text.text   = qsTr("(C) 2014 Aseman")
        about_home.text           = qsTr("Home Page")
        title.text                = qsTr("About")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

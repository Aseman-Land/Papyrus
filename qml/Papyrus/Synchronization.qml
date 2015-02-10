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
    width: 100
    height: 62

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Column {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10*Devices.density
        anchors.verticalCenter: parent.verticalCenter
        spacing: 10*Devices.density

        SynchronizationItem {
            image: "files/dropbox.png"
            text: sync.tokenAvailable? qsTr("Click to disconnect") : qsTr("Sync using Dropbox")
            onClicked: {
                if( sync.tokenAvailable )
                    sync.stop();
                else
                    showSubMessage(Qt.createComponent("DropBoxAuthorize.qml"))
            }
        }

        Rectangle {
            id: file_sync
            width: parent.width
            height:  60*Devices.density
            color: press? "#3B97EC" : "#00000000"
            visible: papyrus.proBuild

            property alias press: marea.pressed

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: file_sync.press? "#ffffff" : "#333333"
                wrapMode: TextInput.WordWrap
                text: qsTr("Sync files")
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: checkbox.checked = !checkbox.checked
            }

            CheckBox {
                id: checkbox
                x: papyrus.languageDirection == Qt.RightToLeft? 20 : file_sync.width - width - 20
                anchors.verticalCenter: parent.verticalCenter
                color: file_sync.press? "#ffffff" : "#333333"
                onCheckedChanged: sync.fileSyncing = checked
                Component.onCompleted: checked = sync.fileSyncing
            }
        }
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text   = qsTr("Synchronization")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

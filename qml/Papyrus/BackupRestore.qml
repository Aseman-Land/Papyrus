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
    id: backup_restore
    width: 100
    height: 62

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    ListView {
        id: backup_restore_list
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20*Devices.density
        highlightMoveDuration: 250
        clip: true
        maximumFlickVelocity: View.flickVelocity

        model: ListModel {}
        delegate: Rectangle {
            id: item
            width: backup_restore_list.width
            height:  50*Devices.density
            color: press? "#3B97EC" : "#00000000"

            property string text: name
            property string command: cmd
            property bool press: false

            Connections{
                target: backup_restore_list
                onMovementStarted: press = false
                onFlickStarted: press = false
            }

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                text: parent.text
                font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: item.press? "#ffffff" : "#333333"
            }

            MouseArea{
                anchors.fill: parent
                onPressed: item.press = true
                onReleased: item.press = false
                onClicked: {
                    if( item.command == "backup" ) {
                        if( database.password().length !== 0 ) {
                            var passItem = getPass(backup_restore)
                            passItem.success.connect(backup_restore.backup)
                            passItem.allowBack = true
                        }
                        else
                            backup_restore.backup()
                    }
                    if( item.command == "restore" ) {
                        var a = restore_dlg_component.createObject(main);
                        main.pushPreference(a)
                    }
                }
            }
        }

        focus: true
        highlight: Rectangle { color: "#3B97EC"; radius: 3; smooth: true }
        currentIndex: -1

        onCurrentItemChanged: {
            if( !currentItem )
                return
        }

        Component.onCompleted: {
            model.clear()

            model.append({"name": qsTr("Make Backup"), "cmd": "backup"})
            model.append({"name": qsTr("Restore"), "cmd": "restore"})

            focus = true
        }
    }

    Component {
        id: restore_dlg_component
        RestoreDialog{
        }
    }

    Component {
        id: backup_wait_component
        BackupWait{
        }
    }

    function backup(){
        var item = backup_wait_component.createObject(main);
        main.pushPreference(item)
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text = qsTr("Backup and Restore")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

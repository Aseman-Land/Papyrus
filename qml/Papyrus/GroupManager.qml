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
import Papyrus 1.0

Item {
    width: 100
    height: 62

    Item {
        id: msg_item
        height: 54*Devices.density
        visible: false

        property string groupId

        Text {
            id: delete_warn
            font.pixelSize: 17*Devices.fontDensity
            font.family: AsemanApp.globalFont.family
            anchors.margins: 10*Devices.density
            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
            color: "#ffffff"
        }

        Button {
            id: yes_button
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: no_button.left
            anchors.rightMargin: 5*Devices.density
            anchors.margins: 10*Devices.density
            width: parent.width/4 - 5*Devices.density
            normalColor: "#aaC80000"
            onClicked: {
                if( msg_item.groupId != "" )
                    database.deleteGroup(msg_item.groupId)

                hideRollerDialog()
                main.refresh()
            }
        }

        Button {
            id: no_button
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 5*Devices.density
            anchors.margins: 10*Devices.density
            width: parent.width/4 - 5*Devices.density
            normalColor: "#66ffffff"
            onClicked: {
                hideRollerDialog()
            }
        }
    }

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    ListView {
        id: groups_list
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20*Devices.density
        anchors.topMargin: 10*Devices.density
        highlightMoveDuration: 250
        clip: true
        maximumFlickVelocity: View.flickVelocity

        model: LabelsModel {}
        delegate: Rectangle {
            id: item
            width: groups_list.width
            height:  50*Devices.density
            color: marea.pressed? "#3B97EC" : "#00000000"

            Rectangle{
                id: clr_item
                width: 15*Devices.density
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 2*Devices.density
                anchors.leftMargin: 10*Devices.density
                color: model.color
            }

            Text{
                id: txt
                anchors.left: clr_item.right
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                text: model.name
                font.pixelSize: 12*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: item.press? "#ffffff" : "#333333"
                horizontalAlignment: Text.AlignLeft
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onPressed: hideRollerDialog()
                onClicked: {
                    var ag = add_dialog_component.createObject(main, {"groupId":model.id})
                    main.pushPreference(ag)
                }
            }

            Button {
                id: delete_btn
                height: parent.height - 10*Devices.density
                width: height
                iconHeight: 22*Devices.density
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.rightMargin: 10*Devices.density
                icon: "files/delete.png"
                normalColor: "#00000000"
                visible: model.id != 0
                onClicked: {
                    msg_item.groupId = model.id
                    showRollerDialog( item.mapToItem(papyrus_root,0,0).y, item.mapToItem(papyrus_root,0,item.height).y, msg_item )
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
    }

    ScrollBar {
        scrollArea: groups_list; height: groups_list.height; width: 6*Devices.density
        anchors.right: groups_list.right; anchors.top: groups_list.top; color: "#000000"
    }

    Connections {
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations() {
        title.text       = qsTr("Label Manager")
        delete_warn.text = qsTr("Are you sure?")
        yes_button.text  = qsTr("Delete")
        no_button.text   = qsTr("Cancel")
    }

    Component.onCompleted: {
        initTranslations()
    }

    Component {
        id: add_dialog_component
        AddGroupDialog {
            onAccepted: {
                main.refresh()
                main.popPreference()
            }
        }
    }
}

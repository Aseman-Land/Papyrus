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
    id: group_selector
    width: 100
    height: 62

    property real itemsHeight: 50*Devices.density

    signal selected( string guid )

    ListView {
        id: groups_list
        anchors.fill: parent
        anchors.bottomMargin: 10*Devices.density
        anchors.topMargin: 10*Devices.density
        highlightMoveDuration: 250
        clip: true
        maximumFlickVelocity: View.flickVelocity

        model: LabelsModel {}

        footer: Rectangle {
            id: item
            width: groups_list.width
            height: group_selector.itemsHeight
            color: marea.pressed? "#3B97EC" : "#00000000"

            Rectangle{
                width: item.width
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 2*Devices.density
                anchors.leftMargin: 0
                color: "#00CD3A"
            }

            Text{
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                anchors.leftMargin: 44*Devices.density
                y: parent.height/2 - height/2
                text: qsTr("Add Label")
                font.pixelSize: 10*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: "#ffffff"
                horizontalAlignment: Text.AlignLeft
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: {
                    group_selector.selected(-1)
                    hideRollerDialog()
                }
            }
        }

        delegate: Rectangle {
            width: groups_list.width
            height: group_selector.itemsHeight
            color: dmarea.pressed? "#3B97EC" : "#00000000"

            Rectangle{
                width: 15*Devices.density
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.left: parent.left
                anchors.margins: 2*Devices.density
                anchors.leftMargin: 10*Devices.density
                color: model.color
            }

            Text{
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                anchors.leftMargin: 44*Devices.density
                y: parent.height/2 - height/2
                text: model.name
                font.pixelSize: 10*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: "#333333"
                horizontalAlignment: Text.AlignLeft
            }

            MouseArea{
                id: dmarea
                anchors.fill: parent
                onClicked: {
                    group_selector.selected(model.id)
                    hideRollerDialog()
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

    function realHeight(){
        return (groups_list.count+1)*itemsHeight
    }
}

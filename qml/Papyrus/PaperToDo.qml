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
import Papyrus 1.0
import AsemanTools 1.0

PapyrusPaper {
    id: paper
    width: 100
    height: 200
    anchors.fill: parent

    property string text: paperCore.text
    property int direction: Tools.directionOf(text)

    onTextChanged: tdmodel.setText(text)

    ToDoListModel {
        id: tdmodel
        onTextChanged: paperCore.text = text
    }

    ListView {
        id: listv
        width: parent.width
        height: Devices.keyboard? parent.height-Devices.keyboardHeight : parent.height
        interactive: paper.interactive
        model: tdmodel

        property int focusOnNew: -1

        delegate: Row {
            width: listv.width
            height: Math.max(txt.height,check_frame.height) + 4*Devices.density
            layoutDirection: paper.direction

            Connections {
                target: listv
                onContentYChanged: if(txt.selectionStart == txt.selectionEnd) txt.hidePicker()
            }

            Item {
                id: check_frame
                anchors.verticalCenter: parent.verticalCenter
                height: 40*Devices.density
                width: height
                visible: model.type == ToDoListModel.TypeCheckable

                Rectangle {
                    id: check
                    anchors.fill: parent
                    anchors.margins: 6*Devices.density
                    radius: height/2
                    border.color: check_check.color
                    border.width: 1*Devices.density
                    color: "#00000000"

                    Rectangle {
                        id: check_check
                        color: database.groupColor(paperCore.group)
                        radius: height/2
                        anchors.fill: parent
                        anchors.margins: 3*Devices.density
                        visible: model.checked
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    onClicked: model.checked = !model.checked
                }
            }

            Item {
                id: normal_pad
                visible: model.type == ToDoListModel.TypeTextOnly
                width: 20*Devices.density
                height: parent.height
            }

            TextLineCore {
                id: txt
                width: check_frame.visible? parent.width-check_frame.width : parent.width-normal_pad.width
                anchors.verticalCenter: parent.verticalCenter
                text: model.text
                onPressChanged: listv.interactive = !press
                onTextChanged: if(inited) model.text = text
                onAccepted: {
                    var newText = text.slice(cursorPosition, length)
                    text = text.slice(0, cursorPosition)
                    listv.focusOnNew = index+1
                    tdmodel.insertLine(index+1, newText)
                }

                property bool inited: false
                Component.onCompleted: inited = true
            }

            Component.onCompleted: {
                if(index == listv.focusOnNew) {
                    listv.focusOnNew = -1
                    txt.cursorPosition = 0
                    txt.focus = true
                }
            }
        }
    }

    ScrollBar {
        scrollArea: listv; height: listv.height; width: 6*Devices.density; anchors.top: listv.top
        anchors.right: txt.horizontalAlignment == Text.AlignRight? parent.left : parent.right
        anchors.rightMargin: txt.horizontalAlignment == Text.AlignRight? -width-3*Devices.density : 3*Devices.density
        z: 20
        color: "#888888"
    }
}

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
    id: paper_todo
    width: 100
    height: 200
    anchors.fill: parent

    property string text: paperCore.text

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
        delegate: Item {
            width: listv.width
            height: txt.height + 12*Devices.density

            TextLineCore {
                id: txt
                width: parent.width
                anchors.verticalCenter: parent.verticalCenter
                text: model.text
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

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
    id: item
    anchors.fill: parent

    property string fileId: fid
    property string path: filePath

    property alias imgItem: back
    property alias paintedHeight: back.height

    signal deleteRequest( variant item )

    onPathChanged: desc.text = Tools.readText(path)

    Rectangle{
        id: back
        anchors.fill: parent
        anchors.margins: 20*Devices.density
        color: "#ffffff"
        radius: 3*Devices.density

        Flickable {
             id: flick
             anchors.fill: back
             anchors.margins: 20*Devices.density

             flickableDirection: Flickable.VerticalFlick

             contentWidth: desc.paintedWidth
             contentHeight: desc.paintedHeight
             clip: true

             function ensureVisible(r)
             {
                 if (contentX >= r.x)
                     contentX = r.x;
                 else if (contentX+width <= r.x+r.width)
                     contentX = r.x+r.width-width;
                 if (contentY >= r.y)
                     contentY = r.y;
                 else if (contentY+height <= r.y+r.height)
                     contentY = r.y+r.height-height;
             }

             STextEdit {
                 id: desc
                 color: "#000000"
                 width: flick.width
                 height: flick.height
                 readOnly: true
                 selectByMouse: false
                 wrapMode: TextEdit.WordWrap
                 onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
             }
         }

        ScrollBar {
            scrollArea: flick; height: flick.height; width: 10
            anchors.right: back.right; anchors.top: flick.top; color: "#333333"
        }
    }
}

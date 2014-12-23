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
import QtGraphicalEffects 1.0
import AsemanTools 1.0

Item {
    id: pback
    width: 100
    height: 62

    property color color: paper_back_rect.color
    property real shadowSize: 15

    Item {
        id: shdw
        anchors.fill: parent
        anchors.margins: -5 - pback.shadowSize
        visible: false

        Rectangle {
            anchors.fill: parent
            anchors.margins: pback.shadowSize - shdw.anchors.margins
            radius: pback.shadowSize-5
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#000000" }
                GradientStop { position: 0.5; color: "#22000000" }
                GradientStop { position: 1.0; color: "#000000" }
            }
        }
    }

    FastBlur {
        anchors.fill: shdw
        anchors.leftMargin: 4
        anchors.topMargin: 4
        source: shdw
        radius: 28
    }

    Rectangle {
        id: paper_back_rect
        anchors.fill: parent
        anchors.margins: pback.shadowSize
        color: "#EDEDED"
    }
}

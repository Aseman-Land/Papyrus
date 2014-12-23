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
    width: txt.width
    height: txt.height

    property alias text: txt.text
    property alias font: txt.font
    property alias color: txt.color
    property alias horizontalAlignment: txt.horizontalAlignment
    property alias verticalAlignment: txt.verticalAlignment

    property alias glowX: mask.glowX
    property alias glowWidth: mask.glowWidth
    property alias glowColor: txt_glow.color

    Text {
        id: txt
    }

    Rectangle {
        id: txt_glow_frame
        anchors.fill: txt
        color: "#00000000"
        visible: false
        smooth: true

        Text {
            id: txt_glow
            anchors.fill: parent
            text: txt.text
            font: txt.font
            color: "#ffffff"
            horizontalAlignment: txt.horizontalAlignment
            verticalAlignment: txt.verticalAlignment
        }
    }

    Rectangle {
        id: mask_frame
        anchors.fill: txt
        color: "#00000000"
        visible: false
        smooth: true

        Rectangle {
            id: mask
            width: txt.height
            height: txt.width
            transformOrigin: Item.Center
            anchors.centerIn: parent
            rotation: -90

            property real glowX: -glowWidth
            property real glowWidth: 20

            property real gradX: mask.glowX/mask.height
            property real gradWidth: glowWidth/mask.height

            gradient: Gradient {
                GradientStop { position: mask.gradX; color: "#00000000" }
                GradientStop { position: mask.gradX+mask.gradWidth/2; color: "#000000" }
                GradientStop { position: mask.gradX+mask.gradWidth; color: "#00000000" }
            }
        }
    }

    ThresholdMask {
        anchors.fill: txt
        source: txt_glow_frame
        maskSource: mask_frame
        threshold: 0.4
        spread: 0.6
    }
}

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

Item {
    id: pback
    width: 100
    height: 62
    smooth: true

    property color color: paper_back_rect.color
    property real shadowSize: 15

    property string text
    property string bodyText
    property alias image: img.source

    onTextChanged: {
        txt.text = ""
        txt_timer.index = 0
        txt_timer.restart()
    }
    onBodyTextChanged: {
        body.text = ""
        body_timer.index = 0
        body_timer.restart()
    }

    Item {
        id: shdw
        anchors.fill: parent
        anchors.margins: -5 - pback.shadowSize
        visible: false
        smooth: true

        Rectangle {
            anchors.fill: parent
            anchors.margins: pback.shadowSize - shdw.anchors.margins
            radius: pback.shadowSize-5
            smooth: true
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
        smooth: true
    }

    Text {
        id: txt
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 30
        font.pointSize: 17
        font.family: "Ubuntu"
        color: "#333333"
    }

    Text {
        id: body
        anchors.top: txt.bottom
        anchors.topMargin: 10
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 30
        font.pointSize: 10
        font.family: "Ubuntu"
        color: "#333333"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
    }

    Timer {
        id: txt_timer
        interval: 65
        onTriggered: {
            txt.text = txt.text + pback.text[index]
            index++
            if( index < pback.text.length )
                restart()
        }

        property int index: 0
    }

    Timer {
        id: body_timer
        interval: 50
        onTriggered: {
            body.text = body.text + pback.bodyText[index]
            index++
            if( index < pback.bodyText.length )
                restart()
        }

        property int index: 0
    }

    Image {
        id: img
        anchors.top: body.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 90
        anchors.bottomMargin: 30
        anchors.topMargin: 30
        opacity: status!=Image.Ready? 0 : 0.7
        scale: status!=Image.Ready? 5 : 1
        transformOrigin: Item.Center
        fillMode: Image.PreserveAspectFit
        verticalAlignment: Image.AlignTop

        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }
}

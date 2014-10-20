/*
    Copyright (C) 2014 Sialan Labs
    http://labs.sialan.org

    Kaqaz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Kaqaz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import QtGraphicalEffects 1.0

Rectangle {
    id: kcmag
    width: 100
    height: width
    radius: width/2
    color: "#333333"
    clip: true
    visible: width > 200*physicalPlatformScale

    property point sourcePositionMap: Qt.point(0,0)
    property alias source: desat.source
    property real magScale: 6

    signal positionChanged( real mouseX, real mouseY )

    Rectangle {
        id: source_frame
        anchors.fill: parent
        clip: true
        visible: false

        Item {
            anchors.fill: parent
            scale: magScale
            transformOrigin: Item.Center

            Desaturate {
                id: desat
                smooth: true
                width: source? source.width : 0
                height: source? source.height : 0
                x: -kcmag.x + sourcePositionMap.x
                y: -kcmag.y + sourcePositionMap.y
            }
        }
    }

    Rectangle {
        id: mask
        anchors.fill: parent
        color: "#00000000"
        smooth: true
        visible: false

        Rectangle {
            anchors.fill: parent
            anchors.margins: 4*physicalPlatformScale
            color: "#ffffff"
            radius: width/2
            transformOrigin: Item.Center
        }
    }

    ThresholdMask {
        anchors.fill: parent
        source: source_frame
        maskSource: mask
        threshold: 0.4
        spread: 0.6
    }

    MouseArea {
        id: marea
        anchors.fill: parent
        onPositionChanged: {
            if( out_of_circle )
                return

            var cX = kcmag.x + (width-width/magScale)/2
            var cY = kcmag.y + (width-width/magScale)/2
            kcmag.positionChanged(cX+mouseX/magScale ,cY+mouseY/magScale )
        }
        onPressed: {
            var cmx = mouseX - width/2
            var cmy = mouseY - height/2

            if( cmx*cmx + cmy*cmy > width*width/4 ) {
                out_of_circle = true
                mouse.accepted = false
                return
            }

            kcmag.positionChanged(-1,-1)
        }
        onReleased: {
            out_of_circle = false
            kcmag.positionChanged(-1,-1)
        }

        property bool out_of_circle: false
    }
}

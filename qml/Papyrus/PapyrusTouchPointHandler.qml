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

Item {
    id: tp_handler

    property variant touchPoint

    signal positionChanged( real x, real y )

    Connections {
        target: touchPoint
        onPressedChanged: update_timer.stop()
        onXChanged: update_timer.startTimer(touchPoint.x,touchPoint.y,true,false)
        onYChanged: update_timer.startTimer(touchPoint.x,touchPoint.y,false,true)
    }

    Timer {
        id: update_timer
        interval: 1
        onTriggered: {
            tp_handler.positionChanged(bx,by)
            x_avlb = false
            y_avlb = false
        }

        property real bx
        property real by

        property bool x_avlb: false
        property bool y_avlb: false

        function startTimer(x,y,xSignal,ySignal) {
            bx = x
            by = y

            if( (xSignal && x_avlb) || (ySignal && y_avlb) ) {
                tp_handler.positionChanged(bx,by)
                x_avlb = false
                y_avlb = false
                stop()
            }

            if( xSignal )
                x_avlb = true
            if( ySignal )
                y_avlb = true

            if( x_avlb && y_avlb ) {
                tp_handler.positionChanged(bx,by)
                x_avlb = false
                y_avlb = false
                stop()
            } else {
                restart()
            }
        }
    }
}

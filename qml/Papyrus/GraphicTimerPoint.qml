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
    id: timer_point
    width: 80*Devices.density
    height: width
    smooth: true

    property bool anim: false
    property bool repeat: false
    property real animSpeed: 1500
    property real delay: 100
    property real endDelay: 100

    property alias color: point.color

    onRotationChanged: {
        if( rotation == 359 )
        {
            anim = false
            rotation = 0
            anim = true
            if( repeat )
                restart_timer.start()
        }
    }

    Behavior on rotation {
        NumberAnimation { easing.type: Easing.InOutQuint; duration: timer_point.anim? animSpeed : 0 }
    }

    Rectangle {
        id: point
        width: 15*Devices.density
        height: width
        x: timer_point.width - width
        y: timer_point.height/2 - height/2
        color: "#3B97EC"
        radius: width/2
    }

    Timer{
        id: timer
        interval: timer_point.delay
        repeat: false
        onTriggered: timer_point.rotation = 359
    }

    Timer{
        id: restart_timer
        interval: timer_point.endDelay
        repeat: false
        onTriggered: timer_point.start()
    }

    function start(){
        timer_point.anim = false
        timer_point.rotation = 0
        timer_point.anim = true
        timer.start()
    }
}

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

Timer {
    id: counter
    repeat: true
    interval: 1

    property int ms: 0
    property real distance: 60*Devices.density

    property real cache: 0
    property real cache_time: 0

    onTriggered: cache_time += 1

    function startCounter(){
        cache_time = 0
        ms = 0
        cache = 0
        start()
    }

    function stopCounter(){
        stop()
        return cache_time
    }

    function addDistance( size ){
        cache += Math.abs(size)
        if( cache < distance )
            return

        ms = cache_time
        cache = 0
    }

    function getTime(){
        return ms
    }

    function speed(){
        return distance/ms
    }
}

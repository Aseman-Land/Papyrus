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
import AsemanTools 1.0

GlowedText {
    id: glow_txt

    property int direction: Qt.LeftToRight
    property real speed: 1000

    QtObject {
        id: privates
        property bool anims: false
    }

    Behavior on glowX {
        NumberAnimation{ easing.type: Easing.Linear; duration: privates.anims? glow_txt.speed : 0 }
    }

    Timer {
        id: reset_timer
        interval: glow_txt.speed*1.7
        onTriggered: glow_txt.start()
    }

    function start() {
        switch(direction)
        {
        case Qt.LeftToRight:
            privates.anims = false
            glowX = -glowWidth
            privates.anims = true
            glowX = glow_txt.width
            reset_timer.restart()
            break;

        case Qt.RightToLeft:
            privates.anims = false
            glowX = glow_txt.width
            privates.anims = true
            glowX = -glowWidth
            reset_timer.restart()
            break;
        }
    }

    function stop() {
        privates.anims = false
        glowX = glow_txt.width
        reset_timer.stop()
    }
}

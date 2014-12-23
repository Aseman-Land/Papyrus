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

AnimationItem {
    id: preference_dialog
    x: 0
    y: 0
    width: parent.width
    height: parent.height

    property variant item
    property variant prevItem

    Behavior on opacity {
        NumberAnimation { easing.type: preference_dialog.easing.type; duration: anim? anim_time : 0 }
    }

    onItemChanged: {
        if( !item )
            return

        item.parent = preference_dialog
        item.x = 0
        item.y = 0
        item.width = width
        item.height = height
        item.visible = true

        if( prevItem )
            prevItem.visible = false
        prevItem = item
    }

    onWidthChanged: {
        if( !item )
            return

        item.width = width
    }

    onHeightChanged:  {
        if( !item )
            return

        item.height = height
    }
}

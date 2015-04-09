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
import Papyrus 1.0

Item {
    id: group_chooser
    width: gc_back.width
    height: gc_back.height
    smooth: true
    clip: true

    property alias radius: gc_back.radius
    property alias color: gc_back.color

    property bool signal_blocker: false

    property PaperCore paperCore
    property variant paperItem: parent

    property bool press: false

    GroupChooserBack {
        id: gc_back
        press: mousearea.pressed
        group: paperCore.group
    }

    MouseArea{
        id: mousearea
        anchors.fill: group_chooser
        onClicked: {
            var paper = group_selector_component.createObject(papyrus_root);
            var point = mapToItem(papyrus_root,width/2,height)
            var h = paper.realHeight()+40*Devices.density
            if( h > 5*papyrus_root.height/9 )
                h = 5*papyrus_root.height/9

            showPointDialog( paper, 0, point.y - 2, 240*Devices.density, h )
            Devices.hideKeyboard()
        }
    }

    Connections{
        target: papyrus
        onLanguageChanged: {
            if( group )
                txt.text = database.groupName(group)
        }
    }

    Component {
        id: group_selector_component
        GroupSelector{
            onSelected: {
                hidePointDialog()
                if( guid == -1 ) {
                    var add_group = add_group_component.createObject(main);
                    main.showDialog(add_group)
                }
                else
                    paperCore.group = guid
            }
        }
    }

    Component {
        id: add_group_component
        AddGroupDialog {
            groupChooserItem: group_chooser
            onAccepted: {
                paperCore.group = guid
                main.closeDialog()
            }
        }
    }
}

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
    id: group_chooser
    width: gc_back.width
    height: gc_back.height
    smooth: true
    clip: true

    property alias radius: gc_back.radius
    property alias color: gc_back.color

    property bool signal_blocker: false

    property variant paperItem: parent
    property alias group: gc_back.group

    property bool press: false

    onGroupChanged: {
        if( !signal_blocker )
            group_chooser.groupSelected(group)
    }

    signal groupSelected( int id )

    Component.onCompleted: {
        signal_blocker = true
        group = 0
        signal_blocker = false
    }

    GroupChooserBack {
        id: gc_back
        press: mousearea.pressed
    }

    MouseArea{
        id: mousearea
        anchors.fill: group_chooser
        onClicked: {
            selectGroup()
            Devices.hideKeyboard()
        }
    }

    function selectGroup(){
        var component = Qt.createComponent("GroupSelector.qml");
        var paper = component.createObject(papyrus_root);
        paper.selected.connect(group_chooser.finished)

        var point = mapToItem(papyrus_root,width/2,height)
        var h = paper.realHeight()+40*Devices.density
        if( h > 5*papyrus_root.height/9 )
            h = 5*papyrus_root.height/9

        showPointDialog( paper, 0, point.y - 2, 240*Devices.density, h )
    }

    function finished( gid ){
        hidePointDialog()
        if( gid == -1 )
        {
            addNewGroup()
        }
        else
        {
            group_chooser.group = gid
            group_chooser.groupSelected(gid)
        }
    }

    function addNewGroup(){
        var component = Qt.createComponent("AddGroupDialog.qml");
        var add_group = component.createObject(main);
        add_group.groupChooserItem = group_chooser
        add_group.accepted.connect(group_chooser.finished)
        add_group.accepted.connect(main.closeDialog)
        main.showDialog(add_group)
    }

    Connections{
        target: papyrus
        onLanguageChanged: {
            if( group )
                txt.text = database.groupName(group)
        }
    }
}

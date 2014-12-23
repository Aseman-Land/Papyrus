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
    id: add_group

    property int groupId
    property variant groupChooserItem

    signal accepted( string guid )

    onGroupIdChanged: {
        if( !groupId )
            return

        txt.text = database.groupName(groupId)
        color_list.currentColor = database.groupColor(groupId)
    }

    Rectangle{
        id: txt_frame
        anchors.left: add_group.left
        anchors.right: add_group.right
        anchors.top: add_group.top
        anchors.topMargin: 20*Devices.density
        anchors.leftMargin: Devices.isMobile && Devices.isAndroid? 40*Devices.density : 90*Devices.density
        anchors.rightMargin: Devices.isMobile && Devices.isAndroid? 40*Devices.density : 90*Devices.density
        height: 75*Devices.density
        radius: 3*Devices.density
        color: color_list.currentColor

        Text{
            id: placeholder_txt
            anchors.fill: txt
            color: "#cccccc"
            visible: (!txt.focus && txt.text == "")
            font: txt.font
            horizontalAlignment: txt.horizontalAlignment
        }

        TextLineCore{
            id: txt
            x: parent.width/2 - width/2
            y: parent.height/2 - height/2
            width: parent.width
            font.pixelSize: 14*Devices.fontDensity
            font.bold: Devices.isWindows? false : true
            font.family: AsemanApp.globalFontFamily
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            inputMethodHints: globalInputMethodHints
            onAccepted: add_group.accept()
        }
    }

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Button{
        id: ok_button
        anchors.left: add_group.left
        anchors.right: add_group.right
        anchors.bottom: add_group.bottom
        anchors.margins: 20*Devices.density
        height: 60*Devices.density
        onClicked: add_group.accept()
        fontSize: 12*Devices.fontDensity
        normalColor: "#66ffffff"
        textColor: "#333333"
    }

    ColorList{
        id: color_list
        anchors.top: txt_frame.bottom
        anchors.left: ok_button.left
        anchors.right: ok_button.right
        anchors.bottom: ok_button.top
        anchors.bottomMargin: 8*Devices.density
        anchors.topMargin: 20*Devices.density
    }

    function accept(){
        var gId = (groupId)? groupId : database.createGroup()
        database.setGroupName(gId, txt.text==""? placeholder_txt.text : txt.text )
        database.setGroupColor( gId, color_list.currentColor )

        if( groupChooserItem )
            groupChooserItem.group = gId

        add_group.accepted( gId )
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        ok_button.text = qsTr("Done")
        placeholder_txt.text = qsTr("New Label")
    }

    Component.onCompleted: initTranslations()
    Component.onDestruction: main.focus = true
}

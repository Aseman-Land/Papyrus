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

Item {
    id: state_page

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10*Devices.density

        Text {
            id: papers_text
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Devices.isMobile? 13*Devices.fontDensity : 15*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
        }

        Text {
            id: group_text
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Devices.isMobile? 13*Devices.fontDensity : 15*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
        }

        Text {
            id: date_text
            anchors.horizontalCenter: parent.horizontalCenter
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            font.pixelSize: Devices.isMobile? 13*Devices.fontDensity : 15*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
        }
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text       = qsTr("States")
        papers_text.text = qsTr("You have %1 Papers").replace("%1",database.statePapersCount())
        group_text.text  = qsTr("that sorts in %1 Labels").replace("%1",database.stateGroupsCount())
        date_text.text   = qsTr("and wrote in %1 Days").replace("%1",database.stateDatesCount())
    }

    Component.onCompleted: {
        initTranslations()
    }
}

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
    id: configure
    width: 100
    height: 62

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    ListView {
        id: prefrences
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20*Devices.density
        highlightMoveDuration: 250
        clip: true

        model: ListModel {}
        delegate: Rectangle {
            id: item
            width: preference_list.width
            height:  50*Devices.density
            color: press? "#3B97EC" : "#00000000"

            property string dstFile: file
            property alias press: marea.pressed
            property bool checkable: check

            property string prprt: pr

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: item.press? "#ffffff" : "#333333"
                wrapMode: TextInput.WordWrap
                text: name
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: {
                    if( !item.checkable ) {
                        var component = Qt.createComponent(item.dstFile)
                        var citem = component.createObject(main)
                        pushPreference(citem)
                    } else {
                        checkbox.checked = !checkbox.checked
                    }
                }
            }

            CheckBox {
                id: checkbox
                x: papyrus.languageDirection == Qt.RightToLeft? 20 : item.width - width - 20
                anchors.verticalCenter: parent.verticalCenter
                visible: item.checkable
                checked: item.prprt.length==0? false : Tools.property(papyrus,item.prprt)
                color: item.press? "#ffffff" : "#333333"
                onCheckedChanged: Tools.setProperty(papyrus,item.prprt,checked)
            }
        }

        highlight: Rectangle { color: "#3B97EC"; radius: 3; smooth: true }
        currentIndex: -1

        function refresh() {
            model.clear()
            model.append({ "name": qsTr("All papers item"), "file": "", "check": true, "pr":"allPaper"})
            model.append({ "name": qsTr("Notes positioning"), "file": "", "check": true, "pr":"positioning"})
            if( papyrus.proBuild )
                model.append({ "name": qsTr("Weather detector"), "file": "", "check": true, "pr":"weatherActive"})
            if( Devices.isMacX )
                model.append({ "name": qsTr("Desktop Touch"), "file": "", "check": true, "pr":"desktopTouchMode"})
            if( !Devices.isDesktop )
                model.append({ "name": qsTr("Fullscreen"), "file": "", "check": true, "pr":"fullscreen"})
            model.append({ "name": qsTr("Word suggestions"), "file": "", "check": true, "pr":"keyboardPredicative"})
            model.append({ "name": qsTr("Groups count"), "file": "", "check": true, "pr":"groupsCount"})
            model.append({ "name": qsTr("Select Map"), "file": "MapSelector.qml", "check": false, "pr":""})
        }
    }

    ScrollBar {
        scrollArea: prefrences; height: prefrences.height; width: 6*Devices.density
        anchors.right: prefrences.right; anchors.top: prefrences.top; color: "#000000"
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        prefrences.refresh()
        title.text = qsTr("General Settings")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

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
    id: profile_settings
    width: 100
    height: 62

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    ListView {
        id: preference_list
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
            color: marea.pressed? "#3B97EC" : "#00000000"

            property string text: name
            property string path: pathValue

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                text: parent.text
                font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                color: marea.pressed? "#ffffff" : "#333333"
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: {
                    if( database.password().length !== 0 ) {
                        var passItem = getPass(backup_restore)
                        passItem.success.connect(move)
                        passItem.allowBack = true
                    }
                    else
                        move()
                }

                function move(){
                    var component = Qt.createComponent("DataMoveWait.qml");
                    var it = component.createObject(main);
                    it.path = item.path
                    main.pushPreference(it)
                }
            }
        }

        focus: true
        highlight: Rectangle { color: "#3B97EC"; radius: 3; smooth: true }
        currentIndex: -1

        onCurrentItemChanged: {
            if( !currentItem )
                return
        }

        Component.onCompleted: {
            model.clear()

            model.append({"name": qsTr("Keep on Device memory"), "pathValue": ""})
            model.append({"name": qsTr("Keep on SD Card"), "pathValue": papyrus.sdcardPath()})

            focus = true
        }
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text = qsTr("Data Location")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

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
    id: size_selector
    width: 100
    height: 62

    property string path

    signal selected( string repID )

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        light: true
    }

    ListView {
        id: options_list
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.bottomMargin: 20*Devices.density
        anchors.topMargin: 10*Devices.density
        highlightMoveDuration: 250
        clip: true

        model: ListModel {}
        delegate: Rectangle {
            id: item
            width: options_list.width
            height:  50*Devices.density
            color: press? "#3B97EC" : "#00000000"

            property string text: optionText
            property int imageWidth: optionWidth
            property bool press: false

            Connections{
                target: options_list
                onMovementStarted: press = false
                onFlickStarted: press = false
            }

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                text: parent.text
                font.pixelSize: 12*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                color: "#ffffff"
            }

            MouseArea{
                anchors.fill: parent
                onPressed: item.press = true
                onReleased: item.press = false
                onClicked: {
                    var id = repository.copyImageToRepository( size_selector.path, imageWidth )
                    size_selector.selected(id)

                    main.popPreference()
                    main.popPreference()
                    main.popPreference()
                    main.darkBackground = false
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

            model.append({"optionText": qsTr("Small"), "optionWidth": 800 })
            model.append({"optionText": qsTr("Normal"), "optionWidth": 1280 })
            model.append({"optionText": qsTr("High"), "optionWidth": 1920 })
            model.append({"optionText": qsTr("Ultra"), "optionWidth": 3000 })

            focus = true
        }
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text = qsTr("Select best size")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

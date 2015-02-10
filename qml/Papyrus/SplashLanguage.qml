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
import QtGraphicalEffects 1.0
import AsemanTools 1.0

Item {
    width: 100
    height: 62

    Text {
        id: title
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: lang_list.top
        anchors.leftMargin: 40*Devices.density
        anchors.rightMargin: 40*Devices.density
        anchors.bottomMargin: 10*Devices.density
        font.pixelSize: 20*Devices.fontDensity
        horizontalAlignment: Text.AlignHCenter
        font.family: AsemanApp.globalFont.family
        color: "#333333"
    }

    ListView {
        id: lang_list
        anchors.margins: 100*Devices.density
        anchors.centerIn: parent
        height: 300*Devices.density
        width: 300*Devices.density
        highlightMoveDuration: 250
        clip: true

        property string lang: papyrus.currentLanguage()

        model: ListModel {}
        header: Item{ width: 10; height: 30*Devices.density }
        footer: Item{ width: 10; height: 30*Devices.density }
        delegate: Rectangle {
            id: item
            width: lang_list.width
            height: 40*Devices.density
            color: marea.pressed || selected? "#883B97EC" : "#00000000"
            radius: 3*Devices.density

            property string text: name
            property bool selected: lang_list.lang == text

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                text: parent.text
                font.pixelSize: 13*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                color: marea.pressed? "#ffffff" : "#333333"
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: {
                    papyrus.setCurrentLanguage(item.text)
                    lang_list.lang = papyrus.currentLanguage()
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

            var langs = papyrus.languages()
            for( var i=0; i<langs.length; i++ )
                model.append({"name": langs[i]})

            focus = true
        }
    }

    ScrollBar {
        scrollArea: lang_list; height: lang_list.height; width: 6*Devices.density
        anchors.left: lang_list.right; anchors.top: lang_list.top;color: "#888888"
    }

    Rectangle {
        id: lang_rect
        anchors.fill: lang_list

        gradient: Gradient {
            GradientStop { position: 0.0; color: "#F4F4F4" }
            GradientStop { position: 0.1; color: "#00000000" }
            GradientStop { position: 0.9; color: "#00000000" }
            GradientStop { position: 1.0; color: "#F4F4F4" }
        }
    }

    function initTranslations() {
        title.text = qsTr("Select Language")
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    Component.onCompleted: initTranslations()
}

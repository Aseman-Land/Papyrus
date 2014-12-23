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
    id: insert_image_dialog
    width: 100
    height: 62

    Image {
        id: back_img
        anchors.fill: parent
        anchors.topMargin: -View.statusBarHeight
        anchors.bottomMargin: -View.navigationBarHeight
        source: "files/menu_background.jpg"
        fillMode: Image.PreserveAspectCrop
    }

    property variant nextPage

    signal selected( string repID )

    onNextPageChanged: {
        if( nextPage ){
            back_img.visible = false
            main.darkBackground = true
        } else {
            back_img.visible = true
            main.darkBackground = false
        }
    }

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        light: true
    }

    ListView {
        id: image_types_list
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
            width: image_types_list.width
            height:  50*Devices.density
            color: press? "#3B97EC" : "#00000000"

            property string fileName: file
            property string command: cmd
            property string text: name
            property bool press: false

            Connections{
                target: image_types_list
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
                font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                color: "#ffffff"
            }

            MouseArea{
                anchors.fill: parent
                onPressed: item.press = true
                onReleased: item.press = false
                onClicked: {
                    if( item.fileName.length != 0 ) {
                        var component = Qt.createComponent(fileName);
                        var obj = component.createObject(main);
                        obj.selected.connect( insert_image_dialog.selected )
                        main.pushPreference(obj)
                        insert_image_dialog.nextPage = obj
                    } else {
                        if( item.command == "picture" ) {
                            Devices.getOpenPictures()
                        }
                    }
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

        Connections{
            target: papyrus
            onLanguageChanged: image_types_list.refresh()
        }

        function refresh(){
            model.clear()
            model.append({"name": qsTr("Camera"), "file": "CameraDialog.qml", "cmd": ""})
            model.append({"name": qsTr("Document"), "file": "DocumentDialog.qml", "cmd": "" })
            model.append({"name": qsTr("Gallery"), "file": "GalleryDialog.qml", "cmd": "picture" })
            model.append({"name": qsTr("Music"), "file": "MusicDialog.qml", "cmd": "" })
            focus = true
        }

        Component.onCompleted: refresh()
    }

    ScrollBar {
        scrollArea: image_types_list; height: image_types_list.height; width: 6*Devices.density
        anchors.right: image_types_list.right; anchors.top: image_types_list.top;color: "#ffffff"
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text = qsTr("Insert Attachment")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

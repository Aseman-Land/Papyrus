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
    id: camera_dialog
    width: 100
    height: 62

    signal selected( string repID )

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        light: true
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
        maximumFlickVelocity: View.flickVelocity

        model: ListModel {}
        delegate: Rectangle {
            id: item
            width: preference_list.width
            height: 100*Devices.density
            color: press? "#3B97EC" : "#00000000"

            property string text: name
            property string path: adrs
            property bool press: false

            onPressChanged: hideRollerDialog()

            Connections{
                target: preference_list
                onMovementStarted: press = false
                onFlickStarted: press = false
            }

            Image{
                id: img
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                anchors.margins: 10*Devices.density
                asynchronous: true
                fillMode: Image.PreserveAspectFit
                width: height*4/3
                sourceSize.width: width
                sourceSize.height: height
                source: "file://" + item.path
            }

            Text{
                id: txt
                anchors.left: img.right
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                text: parent.text
                font.pixelSize: 12*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                color: "#ffffff"
                clip: true
            }

            MouseArea{
                anchors.fill: parent
                onPressed: item.press = true
                onReleased: item.press = false
                onClicked: {
                    var component = Qt.createComponent("ImageSizeSelector.qml");
                    var i = component.createObject(main);
                    i.path = item.path
                    i.selected.connect(camera_dialog.selected)
                    main.pushPreference(i)
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

        Component.onCompleted: refresh()

        function refresh(){
            model.clear()

            var files = papyrus.findEntryFiles(Devices.cameraLocation,["*.jpg","*.JPEG","*.jpeg","*.JPG"])
            for( var i=0; i<20 && i<files.length ; i++ )
                model.append({"name": Tools.fileName(files[i]), "adrs": files[i]})

            focus = true
        }
    }

    ScrollBar {
        scrollArea: preference_list; height: preference_list.height; width: 6*Devices.density
        anchors.right: preference_list.right; anchors.top: preference_list.top;color: "#ffffff"
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        title.text = qsTr("Last Photos")
    }

    Component.onCompleted: {
        initTranslations()
        if( Devices.startCameraPicture() )
            return
        else
        if( !Devices.isTouchDevice ) {
            var component = Qt.createComponent("CameraItem.qml");
            var item = component.createObject(camera_dialog);
        }
    }

    Connections{
        target: filesystem
        onDirectoryChanged: preference_list.refresh()
    }
}

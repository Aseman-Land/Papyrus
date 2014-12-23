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
    id: attach_viewer
    anchors.fill: parent
    opacity: 0
    clip: true

    property int paperItem: -1
    property variant paper

    onPaperItemChanged: {
        attach_list.refresh()
    }

    signal editRequest( string fid )

    function deleteRequest( it ){
        msg_item.fileID = it.fileId
        showRollerDialog( it.mapToItem(papyrus_root,0,0).y + it.height/2 - it.paintedHeight/2,
                         it.mapToItem(papyrus_root,0,0).y + it.height/2 + it.paintedHeight/2, msg_item )
        it.controlFrame = false
    }

    Behavior on opacity {
        NumberAnimation { easing.type: Easing.OutCubic; duration: 250 }
    }

    Item {
        id: msg_item
        height: 54*Devices.density
        visible: false

        property string fileID

        Text {
            id: delete_warn
            font.pixelSize: 17*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            anchors.margins: 10*Devices.density
            anchors.left: parent.left
            anchors.right: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            horizontalAlignment: Text.AlignHCenter
            color: "#ffffff"
        }

        Button {
            id: yes_button
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: no_button.left
            anchors.rightMargin: 5*Devices.density
            anchors.margins: 10*Devices.density
            width: parent.width/4 - 5*Devices.density
            normalColor: "#aaC80000"
            onClicked: {
                database.removeFileFromPaper(paperItem,msg_item.fileID)
                papyrus.deleteFileIfPossible(msg_item.fileID)
                hideRollerDialog()
                attach_list.refresh()
            }
        }

        Button {
            id: no_button
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            anchors.rightMargin: 5*Devices.density
            anchors.margins: 10*Devices.density
            width: parent.width/4 - 5*Devices.density
            normalColor: "#66ffffff"
            onClicked: {
                hideRollerDialog()
            }
        }
    }

    ListView {
        id: attach_list
        anchors.fill: parent
        anchors.topMargin: 10*Devices.density
        anchors.bottomMargin: 10*Devices.density
        highlightMoveDuration: 250
        orientation: ListView.Horizontal
        spacing: 10*Devices.density
        maximumFlickVelocity: View.flickVelocity
        leftMargin: spacing
        rightMargin: spacing
        cacheBuffer: width

        onMovementStarted: attach_list.edited = ""
        onFlickStarted: attach_list.edited = ""

        property string edited

        model: ListModel {}
        delegate: Rectangle {
            id: item
            width: attach_list.width<4*parent.height/3? attach_list.width : imgWidth
            height: parent.height
            color: "#00000000"

            property string fileId: fid
            property string path: filePath

            property variant viewer
            property alias controlFrame: control_frame

            property real imgHeight: 0
            property real imgWidth: attach_list.width
            property real imgY: 0
            property real imgX: 0

            function itemWidthChanged(){
                imgWidth = viewer.width
            }
            function itemHeightChanged(){
                imgHeight = viewer.height
            }
            function itemXChanged(){
                imgX = viewer.x
            }
            function itemYChanged(){
                imgY = viewer.y
            }

            MouseArea{
                anchors.fill: parent
                cursorShape: Qt.PointingHandCursor
                onClicked: attach_list.edited = item.fileId
                onPressed: hideRollerDialog()
                onDoubleClicked: {
                    var p = papyrus.cacheFile(item.path)
                    Devices.openFile( "file://" + p )
                }
            }

            Row {
                id: control_frame
                y: item.imgY + item.imgHeight - height - 2*Devices.density
                height: 30*Devices.density
                spacing: 10*Devices.density
                anchors.horizontalCenter: item.horizontalCenter
                visible: attach_list.edited == item.fileId
                z: 100

                Button {
                    id: open_btn
                    icon: "files/open.png"
                    width: height
                    anchors.top: control_frame.top
                    anchors.bottom: control_frame.bottom
                    radius: 2*Devices.density
                    normalColor: "#333333"
                    highlightColor: "#222222"
                    onClicked: {
                        var p = papyrus.cacheFile(item.path)
                        Devices.openFile( "file://" + p )
                        control_frame.visible = false
                    }
                }

                Button {
                    id: edit_btn
                    icon: "files/pen.png"
                    width: height
                    anchors.top: control_frame.top
                    anchors.bottom: control_frame.bottom
                    radius: 2*Devices.density
                    normalColor: "#333333"
                    highlightColor: "#222222"
                    visible: false
                    onClicked: {
                        attach_viewer.editRequest(fileId)
                    }
                }

                Button {
                    id: delete_btn
                    icon: "files/delete.png"
                    width: height
                    anchors.top: control_frame.top
                    anchors.bottom: control_frame.bottom
                    radius: 2*Devices.density
                    normalColor: "#333333"
                    highlightColor: "#222222"
                    onClicked: {
                        msg_item.fileID = item.fileId
                        showRollerDialog( item.mapToItem(papyrus_root,0,item.imgY).y,
                                          item.mapToItem(papyrus_root,0,item.imgY).y + item.imgHeight, msg_item )
                        control_frame.visible = false
                    }
                }
            }

            Component.onCompleted: {
                var suffix = Tools.fileSuffix(item.path)
                var component;
                if( suffix === "jpg" || suffix === "jpeg" || suffix === "png" ) {
                    component = Qt.createComponent("ImageViewer.qml");
                    edit_btn.visible = papyrus.proBuild
                }
                else
                if( suffix === "mp3" || suffix === "wav" || suffix === "ogg" )
                    component = music_viewer_component;
                else
                if( suffix === "txt" || suffix === "text" )
                    component = Qt.createComponent("TextViewer.qml");
                else
                if( suffix === "pdf" )
                    component = Qt.createComponent("PdfViewer.qml");

                var it = component.createObject(item);
                it.fileId = item.fileId
                it.path = item.path
                viewer = it

                it.deleteRequest.connect( attach_viewer.deleteRequest )
                it.widthChanged.connect( item.itemWidthChanged )
                it.heightChanged.connect( item.itemHeightChanged )
                it.xChanged.connect( item.itemXChanged )
                it.yChanged.connect( item.itemYChanged )

                if( it.width != 0 )  imgWidth = it.width
                if( it.height != 0 ) imgHeight = it.height
                imgX = it.x
                imgY = it.y
            }
        }

        focus: true
        highlight: Rectangle { color: "#3B97EC"; radius: 3; smooth: true }
        currentIndex: -1

        function refresh() {
            if( !model )
                return

            model.clear()
            if( attach_viewer.paperItem == -1 )
                return

            var files = database.paperFiles(attach_viewer.paperItem)
            for( var i=0; i<files.length; i++ )
                model.append({ "fid": files[i], "filePath": repository.getPath(files[i])})

            focus = true
        }

        onCurrentItemChanged: {
            if( !currentItem )
                return
        }

        Component.onCompleted: refresh()
    }

    ScrollBar {
        scrollArea: attach_list; width: attach_list.width; height: 8
        anchors.bottom: parent.bottom; anchors.bottomMargin: 8*Devices.density;
        color: "#ffffff"; orientation: Qt.Horizontal
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    Component {
        id: music_viewer_component
        MusicViewer{
            onPressedChanged: attach_list.interactive = !pressed
        }
    }

    function initTranslations(){
        delete_warn.text = qsTr("Are you sure?")
        yes_button.text  = qsTr("Delete")
        no_button.text   = qsTr("Cancel")
    }

    Component.onCompleted: {
        opacity = 1
        initTranslations()
    }
}

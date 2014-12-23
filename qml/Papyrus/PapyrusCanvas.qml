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
    id: kcanvas
    width: 100
    height: 62

    signal done( string fileName, string fileId )

    property string fileId

    ListObject {
        id: list
    }

    QtObject {
        id: privates
        property variant magHelper
    }

    Image {
        id: img
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: tools.top
        anchors.bottomMargin: -10*Devices.density
        fillMode: Image.PreserveAspectFit
        sourceSize: Qt.size(width,height)
        source: fileId.length!=0? "file://" + repository.getPath(fileId) : ""
        asynchronous: true
        visible: false
        smooth: true
        onStatusChanged: {
            if( status != Image.Ready )
                return

            canvas.load_image = true
            canvas.requestPaint()
        }
    }

    PaperBackground {
        id: paper_back
        x: fileId.length==0? 0 : parent.width/2 - img.paintedWidth/2
        y: fileId.length==0? 0 : parent.height/2 - img.paintedHeight/2 - 20*Devices.density
        width: fileId.length==0? parent.width : img.paintedWidth
        height: fileId.length==0? tools.y + 10*Devices.density : img.paintedHeight
        scale: mag.visible? 0.7 : 1
        transformOrigin: Item.Center

        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }

        Canvas {
            id: canvas
            anchors.fill: parent
            anchors.margins: 15
            renderStrategy: Canvas.Threaded
            smooth: true

            property bool newLine: false
            property bool endLine: false
            property bool start: false

            property color strokeColor: "black"
            property real strokeWidth: 1
            property int penMode: 0

            property bool load_image: false

            onPaint: {
                var ctx = canvas.getContext("2d");
                if( !start ) {
                    ctx.fillStyle = paper_back.color
                    ctx.fillRect(0,0,width,height)
                    start = true
                    return
                }
                if( load_image ) {
                    ctx.drawImage( img,0,0, width, height )
                    load_image = false
                    return
                }

                ctx.strokeStyle = penMode==2? paper_back.color : strokeColor
                ctx.fillStyle = penMode==2? paper_back.color : strokeColor
                ctx.lineWidth = penMode==2? 20*Devices.density : strokeWidth*Devices.density

                while( !list.isEmpty() ) {
                    var item = list.takeFirst()
                    ctx.beginPath()
                    ctx.arc(item.x0, item.y0, strokeWidth/2, 0, 2*Math.PI, true)
                    ctx.fill()

                    ctx.beginPath()
                    ctx.moveTo(item.x0,item.y0)
                    ctx.lineTo(item.x1,item.y1)
                    ctx.stroke()
                    newLine = false
                }
            }

            Component.onCompleted: requestPaint()
        }
    }


    MultiPointTouchArea {
        id: tarea
        anchors.fill: parent
        minimumTouchPoints: 1
        maximumTouchPoints: 2
        touchPoints: [
            TouchPoint {
                id: point1
                onPressedChanged: {
                    if( !point2.pressed )
                        updatePos(-1,-1)
                    if( pressed )
                        mag_size_changed = false
                    if( !pressed && !mag_size_changed )
                        mag.width = 0
                }
                property bool mag_size_changed: false
            },
            TouchPoint {
                id: point2
                onXChanged: tarea.calculateMagSize()
                onYChanged: tarea.calculateMagSize()
                onPressedChanged: updatePos(-1,-1)
            }
        ]

        function calculateMagSize() {
            if( !point1.pressed || !point2.pressed )
                return

            var w = Math.pow( Math.pow(point1.x-point2.x,2)+Math.pow(point1.y-point2.y,2), 0.5 )
            if( w > Math.min(kcanvas.width,kcanvas.height)*4/5 )
                w = Math.min(kcanvas.width,kcanvas.height)*4/5

            var x = point1.x<point2.x? point1.x : point2.x
            var y = (point1.y<point2.y? point1.y : point2.y)

            mag.x = x - (w - Math.abs(point1.x-point2.x))/2
            mag.y = y - (w - Math.abs(point1.y-point2.y))/2
            mag.width = w
            point1.mag_size_changed = true
        }

        PapyrusTouchPointHandler {
            id: handler1
            touchPoint: point1
            onPositionChanged: {
                if( point2.pressed )
                    return

                if( !mag.visible )
                    updatePos(x,y)

                tarea.calculateMagSize()
            }
        }
    }

    PapyrusCanvasMaginifier {
        id: mag
        source: paper_back
        onPositionChanged: updatePos(mouseX,mouseY)
        sourcePositionMap: Qt.point(paper_back.x,paper_back.y)
        onVisibleChanged: {
            if( visible && privates.magHelper ) {
                privates.magHelper.destroy()
                papyrus.canvasHelperFirstTime = false
            }
        }
    }

    Row {
        id: tools
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        height: 60*Devices.density
        spacing: 8*Devices.density
        anchors.leftMargin: 8*Devices.density
        anchors.rightMargin: 8*Devices.density

        Button {
            width: 80*Devices.density
            height: 40*Devices.density
            anchors.verticalCenter: parent.verticalCenter
            text: qsTr("Done")
            icon: "files/done.png"
            normalColor: "#66ffffff"
            textColor: "#333333"
            onClicked: {
                var fileName = papyrus.getTempPath() + ".png"
                canvas.save(fileName)

                kcanvas.done(fileName, fileId)

                papyrus.removeFile(fileName)
                main.closeDialog()
            }
        }

        Button {
            width: 40*Devices.density
            height: width
            anchors.verticalCenter: parent.verticalCenter
            icon: "files/pen-color.png"
            iconHeight: 24*Devices.density
            border.color: canvas.strokeColor
            border.width: 1*Devices.density
            normalColor: "#44ffffff"
            onClicked: {
                var pos = mapToItem(papyrus_root,0,0)
                showPointDialog( color_component.createObject(tools), pos.x - width, pos.y, 250*Devices.density, 300*Devices.density )
            }
        }

        Button {
            width: 40*Devices.density
            height: width
            anchors.verticalCenter: parent.verticalCenter
            icon: "files/pen-width.png"
            iconHeight: 24*Devices.density
            normalColor: "#44ffffff"
            onClicked: {
                var pos = mapToItem(papyrus_root,0,0)
                showPointDialog( pen_width_component.createObject(tools), pos.x - width, pos.y, 250*Devices.density, 300*Devices.density )
            }
        }

        Button {
            width: 40*Devices.density
            height: width
            anchors.verticalCenter: parent.verticalCenter
            normalColor: "#44ffffff"
            icon: "files/pen-mode.png"
            iconHeight: 24*Devices.density
            onClicked: {
                var pos = mapToItem(papyrus_root,0,0)
                showPointDialog( pen_mode_component.createObject(tools), pos.x - width, pos.y, 250*Devices.density, 200*Devices.density )
            }
        }

        Button {
            width: 40*Devices.density
            height: width
            anchors.verticalCenter: parent.verticalCenter
            normalColor: "#44ffffff"
            icon: "files/pen-magnifier.png"
            iconHeight: 24*Devices.density
            visible: !Devices.isDesktop
            onClicked: {
                var pos = mapToItem(papyrus_root,0,0)
                showPointDialog( mag_level_component.createObject(tools), pos.x - width, pos.y, 250*Devices.density, 200*Devices.density )
            }
        }
    }

    Component {
        id: canvas_mhelper_component
        PapyrusCanvasMagnifierHelper {
            anchors.fill: parent
            anchors.bottomMargin: -View.navigationBarHeight
            anchors.topMargin: -View.statusBarHeight
        }
    }

    Component {
        id: color_component
        PapyrusCanvasColorList {
            selectedColor: canvas.strokeColor
            onSelectedColorChanged: canvas.strokeColor = selectedColor
        }
    }

    Component {
        id: pen_width_component
        PapyrusCanvasPenWidth {
            selectedPen: canvas.strokeWidth
            onSelectedPenChanged: canvas.strokeWidth = selectedPen
        }
    }

    Component {
        id: pen_mode_component
        PapyrusCanvasPenMode {
            selectedMode: canvas.penMode
            onSelectedModeChanged: canvas.penMode = selectedMode
        }
    }

    Component {
        id: mag_level_component
        PapyrusCanvasMagnifierLevel {
            selectedLevel: mag.magScale
            onLevelSelected: {
                if( papyrus.canvasHelperFirstTime && !privates.magHelper )
                    privates.magHelper = canvas_mhelper_component.createObject(kcanvas)

                mag.magScale = level
            }
        }
    }

    property real startX: -1
    property real startY: -1
    function updatePos( mX, mY ) {
        if( mX == -1 && mY == -1 ) {
            startX = -1
            startY = -1
            return
        }

        var cgpos = mapToItem(canvas,mX,mY)
        if( startX == -1 || startY == -1 ) {
            startX = cgpos.x
            startY = cgpos.y
        } else {
            list.append(  {"x0":startX,"y0":startY,"x1":cgpos.x,"y1":cgpos.y} )
            if( canvas.penMode != 1 ) {
                startX = cgpos.x
                startY = cgpos.y
            }

            canvas.requestPaint()
        }
    }
}

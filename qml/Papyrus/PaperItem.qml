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
import QtGraphicalEffects 1.0
import AsemanTools 1.0
import Papyrus 1.0

AnimationItem {
    id: paper
    width: parent.width
    height: fixedHeight
    z: 100+paperZ
    scale: paperTrigger? 1+5*Devices.density/height : 1
    anim_time: 400

    property int paperZ: 0
    property bool paperTrigger

    property alias text: core.title
    property alias font: txt.font

    property string bodyText: core.text
    property alias groupColor: group_chooser.color

    property real pad: -5
    property real closeX: -20 - pad -width
    property bool stayOut: false

    property bool gestures: true
    property bool fake: false

    property alias paperItem: core.paperId

    signal closed( variant p )
    signal entered( variant p )
    signal deleted( variant p )

    signal outMove( variant size )
    signal outMoveFinished()
    signal saved( int id )

    onAnimationFinished: tempDuration = 0

    onWidthChanged: {
        if( stayOut )
            x = 20 + pad - width
    }

    function save(){
        paper.saved(paper_item)
    }

    function deleteRequest(){
        paper.deleted(paper)
    }

    function outMoveFnc( size ){
        paper.anim = false
        paper.x = size-paper.width
        paper.opacity = 1
        paper.anim = true
        paper.paperTrigger = true
    }

    function outMoveFinishedFnc(){
        paper.paperTrigger = false
        if( paper.x+paper.width < paper.width/3 )
            paper.x = closeX
        else
        {
            paper.x = 0
            startAnimation()
            paper.entered(paper)
        }
    }

    function addFile( url ){
        var id = repository.insert( url )
        attach_menu.selected(id)
    }

    function showAttachments(){
        attach_menu.opened = true
    }

    Behavior on opacity {
        NumberAnimation { easing.type: Easing.OutCubic; duration: anim? 1000 : 0 }
    }

    Behavior on scale {
        NumberAnimation { easing.type: Easing.OutCubic; duration: 250 }
    }

    PaperBackground {
        id: paper_back
        anchors.fill: parent
    }

    DistanceCounter {
        id: counter
    }

    PaperCore {
        id: core
        onTitleChanged: txt.text = title
    }

    Item {
        id: flick_frame
        anchors.fill: parent
        anchors.topMargin: txt_frame.y + txt.height
        anchors.bottomMargin: 20+pad + 45*Devices.density
        z: 20
        clip: true

        Rectangle {
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20*Devices.density
            anchors.rightMargin: 20*Devices.density
            height: 15*Devices.density
            z: 20
            gradient: Gradient {
                GradientStop { position: 0.0; color: paper_back.color }
                GradientStop { position: 1.0; color: "#00000000" }
            }
        }

        Rectangle {
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20*Devices.density
            anchors.rightMargin: 20*Devices.density
            height: 15*Devices.density
            z: 20
            gradient: Gradient {
                GradientStop { position: 1.0; color: paper_back.color }
                GradientStop { position: 0.0; color: "#00000000" }
            }
        }
    }

    Item {
        id: txt_frame
        anchors.top: paper.top
        anchors.left: paper.left
        anchors.right: paper.right
        anchors.topMargin: 20+pad + (Devices.isMobile? 5*Devices.density : 15*Devices.density)
        anchors.leftMargin: group_chooser.x + group_chooser.width
        anchors.rightMargin: group_chooser.x + group_chooser.width
        height: 200*Devices.density
        clip: true
        z: 20

        STextInput{
            id: placeholder_txt
            anchors.fill: txt
            font: txt.font
            color: "#cccccc"
            horizontalAlignment: TextInput.AlignHCenter
            visible: (!txt.focus && txt.text == "" && txt.visible)
        }

        TextLineCore{
            id: txt
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right
            font.pixelSize: Devices.isLargeTablet? 15*Devices.fontDensity : (Devices.isMobile? 12*Devices.fontDensity : 14*Devices.fontDensity)
            font.family: AsemanApp.globalFont.family
            color: "#333333"
            selectionColor: "#0d80ec"
            selectedTextColor: "#ffffff"
            horizontalAlignment: TextInput.AlignHCenter
            inputMethodHints: globalInputMethodHints
            selectByMouse: true
            onTextChanged: core.title = text
        }
    }

    GroupChooser {
        id: group_chooser
        anchors.top: paper.top
        anchors.left: paper.left
        anchors.topMargin: 20+pad
        anchors.leftMargin: 20+pad + 15*Devices.density
        z: 20
        paperCore: core
    }

    Text {
        id: date_label
        anchors.bottom: paper.bottom
        anchors.left: parent.left
        anchors.margins: 20+pad + 7*Devices.density
        font.pixelSize: 7*Devices.fontDensity
        font.family: AsemanApp.globalFont.family
        color: "#aaaaaa"
        text: core.paperId == -1? "" : CalendarConv.convertDateTimeToString(core.create)
    }

    AttachMenu {
        id: attach_menu
        anchors.bottom: paper.bottom
        anchors.left: paper.left
        anchors.right: paper.right
        paper: paper
        paperCore: core
        paperIsEmpty: date_label.text.length==0
        z: 30
        onOpenedChanged: pasteButton.textItem = 0
    }

    Connections {
        target: papyrus
        onLanguageChanged: initTranslations()
        onCalendarChanged: initTranslations()
    }

    function initTranslations(){
        placeholder_txt.text = qsTr("Title")
    }

    Component.onCompleted: {
        initTranslations()
    }
}

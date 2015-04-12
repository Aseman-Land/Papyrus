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
    anim_time: 600

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

    onWidthChanged: {
        if( stayOut )
            x = 20 + pad - width
    }

    function save() {
        core.save()
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

    PaperCore {
        id: core
        onTitleChanged: txt.text = title
    }

    PaperDragArea {
        id: flick_frame
        anchors.fill: parent
        anchors.topMargin: txt_frame.y + txt.height
        anchors.bottomMargin: 20+pad + 45*Devices.density
        clip: true
        onPressed: {
            Devices.hideKeyboard()
            last_x = mouseX
            first_x = paper.x
            last_x_size = 0
        }
        onReleased: {
            startAnimation()
            if( paper.x == 0 )
                paper.outMoveFinished()
            else
            if( paper.x < 0 )
            {
                if( (last_x_size < (first_x==0?1:-1) && paper.x < -main.width/4) || paper.x < -3*main.width/4 )
                {
                    paper.x = closeX
                    paper.stayOut = true
                    paper.closed(paper)
                }
                else
                {
                    paper.x = 0
                    paper.stayOut = false
                    if( first_x == closeX )
                        paper.entered(paper)
                }
                paper.paperTrigger = false
            }
            else
            {
                paper.x = 0
            }
        }

        onMouseXChanged: {
            pasteButton.textItem = 0
            var sz = mouseX-last_x
            last_x_size = (sz == 0)? last_x_size : sz

            paper.x = paper.x + sz
            if( paper.x > 0 )
            {
                paper.outMove(sz)
                paper.x = 0
                paper.paperTrigger = false
            }
            else
                paper.paperTrigger = true
        }

        property real last_x
        property real first_x
        property real last_x_size: 0

        Item {
            id: paper_frame
            height: parent.height
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20*Devices.density
            anchors.rightMargin: 20*Devices.density

            property variant item: {
                var component
                var map = {"paperCore": core}
                switch(core.type)
                {
                case Enums.ToDo:
                    component = Qt.createComponent("PaperToDo.qml")
                    break;

                default:
                case Enums.Normal:
                    component = Qt.createComponent("PaperNormalText.qml")
                    break;
                }

                return component.createObject(paper_frame, map)
            }
            property variant oldItem

            onItemChanged: {
                if(oldItem)
                    oldItem.destroy()

                oldItem = item
            }
        }

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

    Row {
        id: btn_box
        height: 30*Devices.density
        anchors.top: parent.top
        anchors.right: parent.right
        anchors.margins: 20+pad + 10*Devices.density
        spacing: 2*Devices.density

        Button{
            id: edit_btn
            width: height
            height: parent.height
            icon: press? "files/edit_hover_64.png" : "files/edit_64.png"
            iconHeight: 32*Devices.density
            normalColor: "#00000000"
            highlightColor: "#00000000"
            opacity: press? 1 : 0.1
            onClicked: {
                main.focus = true
                paper.edit()
            }
        }

        Button {
            id: menu_btn
            width: height
            height: parent.height
            icon: press? "files/menu_hover_64.png" : "files/menu_64.png"
            iconHeight: 32*Devices.density
            normalColor: "#00000000"
            highlightColor: "#00000000"
            opacity: press? 1 : 0.1
            visible: portrait
            onClicked: {
                showMenu()
                main.focus = true
                Devices.hideKeyboard()
            }
        }
    }

    Button {
        id: listview_btn
        height: 30*Devices.density
        width: height
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.margins: 20+pad + 10*Devices.density
        icon: press? "files/listview_hover.png" : "files/listview.png"
        iconHeight: 30*Devices.density
        normalColor: "#00000000"
        highlightColor: "#00000000"
        opacity: press? 1 : 0.2
        z: 20
        onClicked: main.showListView()
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

    function edit(){
        main.closePanel()
        var msg = showSubMessage(paper_edit_component)
        Devices.hideKeyboard()
    }

    Component.onCompleted: {
        initTranslations()
    }

    Component {
        id: paper_edit_component
        PaperEditDialog {
            paperItem: paper
        }
    }
}

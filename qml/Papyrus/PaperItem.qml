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

AnimationItem {
    id: paper
    width: parent.width
    height: fixedHeight
    z: 100+paperZ
    scale: paperTrigger? 1+5*Devices.density/height : 1
    anim_time: tempDuration? tempDuration : 400

    property int paperZ: 0
    property int tempDuration: 0

    property bool paperTrigger: false

    property alias text: txt.text
    property alias font: txt.font

    property alias bodyText: label.text
    property alias groupColor: group_chooser.color

    property real pad: -5
    property real closeX: -20 - pad -width
    property bool stayOut: false

    property bool gestures: true
    property bool signal_blocker: false
    property bool fake: false

    property int paperItem: -1

    signal closed( variant p )
    signal entered( variant p )
    signal deleted( variant p )

    signal outMove( variant size )
    signal outMoveFinished()
    signal saved( int id )

    onAnimationFinished: tempDuration = 0

    onWidthChanged:{
        if( stayOut )
            x = 20 + pad - width
    }

    onPaperItemChanged: {
        save_timer.stop()
        if( fake )
            return

        signal_blocker = true
        if( paperItem == -1 )
        {
            if( !papyrus )
            {
                signal_blocker = false
                return
            }

            group_chooser.group = 0
            txt.text = ""
            date_label.text = ""
            label.text = ""
        }
        else
        {
            txt.text = database.paperTitle(paperItem)
            group_chooser.group = database.paperGroup(paperItem)
            date_label.text = CalendarConv.convertDateTimeToString( database.paperCreatedDate(paperItem) )
            label.text = database.paperText(paperItem)
        }
        save_timer.stop()
        signal_blocker = false
    }

    function save(){
        if( fake  )
            return
        if( signal_blocker )
            return

        var paper_item = paperItem
        if( paperItem == -1 )
            paper_item = database.createPaper()

        database.setPaper( paper_item, txt.text, label.text, group_chooser.group )
        if( paperItem == -1 ) {
            database.setPaperLocation(paper_item, position.coordinate)
            database.setPaperTemperature( paper_item, weather.temperature )
            database.setPaperWeather( paper_item, weather.weatherIcon )
        }

        paperItem = paper_item
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

    function focusOnLabel(){
        label.focusOn(0,0)
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

    Timer{
        id: save_timer
        interval: 1000
        repeat: false
        onTriggered: save()
    }

    PaperBackground {
        id: paper_back
        anchors.fill: parent
    }

    DistanceCounter {
        id: counter
    }

    Item {
        id: flick_frame
        anchors.fill: parent
        anchors.topMargin: txt_frame.y + txt.height
        anchors.bottomMargin: 20+pad + 45*Devices.density
        z: 20
        clip: true

        Flickable {
            id: label_flickable
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.leftMargin: 20+pad + (Devices.isMobile? 20*Devices.density : 25*Devices.density)
            anchors.rightMargin: 20+pad + (Devices.isMobile? 20*Devices.density : 25*Devices.density)
            height: parent.height
            contentWidth: label.width
            contentHeight: Devices.keyboard && !Devices.isDesktop? label.paintedHeight+Devices.keyboardHeight : label.paintedHeight+25*Devices.density
            flickableDirection: Flickable.VerticalFlick
            interactive: !label.pickersPressed && secondInteractive
            onMovementStarted: {
                label.commitBlocker = true
                pasteButton.textItem = 0
                hideRollerDialog()
            }
            onMovementEnded: {
                if(label.selectionStart != label.selectionEnd ) {
                    label.commitBlocker = false
                    label.commitFaders()
                }
            }
            onContentHeightChanged: if(Devices.keyboard) label_flickable.ensureVisible(label.cursorPosition)

            property bool secondInteractive: true

            function ensureVisible(pos)
            {
                var r = label.positionToRectangle(pos)
                var hg = paper.height - Devices.keyboardHeight - 30*Devices.density
                if( label.pickersPressed || label.selectionStart != label.selectionEnd )
                    hg = height
                if( Devices.isDesktop )
                    hg = height

                if (contentY >= r.y)
                    contentY = r.y;
                else if (contentY+hg <= r.y+r.height+15*Devices.density)
                    contentY = r.y+r.height-hg + 15*Devices.density;
            }

            PaperLabel {
                id: label
                width: label_flickable.width
                height: smaller? label_flickable.height : paintedHeight+15*Devices.density
                paperItem: paper
                resizeOnFull: false
                onSelectionStartChanged: label_flickable.ensureVisible(label.selectionStart)
                onSelectionEndChanged: label_flickable.ensureVisible(label.selectionEnd)
                onHeightChanged: if( label.selectionStart == label.selectionEnd ) label_flickable.ensureVisible(label.cursorPosition)

                property bool smaller: label_flickable.height > paintedHeight+15*Devices.density

                onTextChanged: {
                    if( paper.signal_blocker )
                        return

                    if( !focus && text == "" )
                        return

                    save_timer.restart()
                }
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

    ScrollBar {
        scrollArea: label_flickable; height: flick_frame.height; width: 6*Devices.density
        anchors.right: label.horizontalAlignment == Text.AlignRight? paper_back.left : paper_back.right
        anchors.rightMargin: label.horizontalAlignment == Text.AlignRight? -width-15-3*Devices.density : 15 + 3*Devices.density
        anchors.top: flick_frame.top
        z: 20
        color: "#888888"
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
            onTextChanged: {
                if( signal_blocker )
                    return

                save_timer.restart()
            }
        }
    }

    GroupChooser{
        id: group_chooser
        anchors.top: paper.top
        anchors.left: paper.left
        anchors.topMargin: 20+pad
        anchors.leftMargin: 20+pad + 15*Devices.density
        paperItem: paper
        z: 20
        onGroupSelected: {
            if( paper.signal_blocker )
                return
            if( paper.paperItem == -1 )
                return
            if( group )
                save_timer.restart()
        }
    }

    Text {
        id: date_label
        anchors.bottom: paper.bottom
        anchors.left: parent.left
        anchors.margins: 20+pad + 7*Devices.density
        font.pixelSize: 7*Devices.fontDensity
        font.family: AsemanApp.globalFont.family
        color: "#aaaaaa"
    }

    AttachMenu {
        id: attach_menu
        anchors.bottom: paper.bottom
        anchors.left: paper.left
        anchors.right: paper.right
        paper: paper
        paperItem: paper.paperItem
        paperIsEmpty: date_label.text.length==0
        z: 30
        onOpenedChanged: pasteButton.textItem = 0
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
        onCalendarChanged: initTranslations()
    }

    Component {
        id: mousearea_component

        MouseArea {
            id: mousearea
            anchors.fill: parent
            anchors.margins: -10*Devices.density
            onPressedChanged: if(!pressed && label.textFocus) mReleased()

            onMouseXChanged: {
                if( paper.anim )
                    return
                if( label.pickersPressed || label.selectedText.length != 0 )
                    return
                if( !pressedValve )
                    return

                if( !gestures )
                    return
                if( Math.abs(last_x-mouseX) > 10*Devices.density && !move_paper_x )
                {
                    Devices.hideKeyboard()
                    press_is_click = false
                    move_paper_x = true
                    pasteButton.textItem = 0
                }
                if( !move_paper_x )
                    return

                if( label_flickable )
                    label_flickable.secondInteractive = false

                var sz = mouseX-last_x
                last_x_size = (sz == 0)? last_x_size : sz
                counter.addDistance(sz)

                paper.x = paper.x + sz
                if( paper.x + sz > 0 )
                {
                    paper.outMove(sz)
                    paper.x = 0
                    paper.paperTrigger = false
                }
                else
                    paper.paperTrigger = true
            }

            onReleased: if( !label.textFocus ) mReleased()

            onPressed: {
                var onPickers = label.isPointOnPickers( label.mapFromItem(mousearea,mouseX,mouseY).x, label.mapFromItem(mousearea,mouseX,mouseY).y )
                if( onPickers || label.selectedText.length != 0 ) {
                    mouse.accepted = false
                    return
                }
                if( label.pickersPressed )
                    return

                counter.startCounter()

                pressedValve = true
                last_x = mouseX
                last_y = mouseY
                last_x_size = 0
                last_y_size = 0
                first_x = paper.x
                first_y = paper.y
                move_paper_y = false

                if( mousearea.parent != label_flickable || !Devices.keyboard )
                    paper.focus = true

                if( paper.x == 0 )
                    press_is_click = true

            }

            function mReleased() {
                if( label.pickersPressed || label.selectedText.length != 0 )
                    return
                if( !pressedValve )
                    return

                pressedValve = false
                label_flickable.secondInteractive = true
                var speed = counter.speed()/10
                counter.stopCounter()

                if( move_paper_x )
                {
                    startAnimation()
                    if( paper.x == 0 )
                        paper.outMoveFinished()
                    else
                    if( paper.x < 0 )
                    {
                        if( (last_x_size < (first_x==0?1:-1) && paper.x < -main.width/4) || paper.x < -3*main.width/4 )
                        {
                            paper.tempDuration = Math.abs(closeX-paper.x)/speed
                            if( paper.tempDuration > 650 )
                                paper.tempDuration = 650
                            if( paper.tempDuration < 200 )
                                paper.tempDuration = 200

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
                else
                if( press_is_click || move_paper_y )
                {
                    if( !label_flickable.dragging )
                        label.focusOn( label.mapFromItem(mousearea,mouseX,mouseY).x, label.mapFromItem(mousearea,mouseX,mouseY).y )
                }
                else
                    Devices.hideKeyboard()

                move_paper_x = false
                press_is_click = false
            }

            Connections {
                target: label_flickable
                onContentYChanged: {
                    mousearea.move_paper_y = true
                    press_is_click = false
                }
            }

            property real last_x: 0
            property real last_y: 0

            property bool move_paper_x: false
            property bool move_paper_y: false

            property real last_x_size: 0
            property real last_y_size: 0

            property real first_x: 0
            property real first_y: 0

            property real mouse_pin_X: 0
            property real mouse_pin_y: 0

            property bool pressedValve: false
            property bool press_is_click: false
        }
    }

    function refreshDateLabel() {
        if( paperItem != -1 )
            date_label.text = CalendarConv.convertDateTimeToString( database.paperCreatedDate(paperItem) )
        else
            date_label.text = ""
    }

    function initTranslations(){
        placeholder_txt.text = qsTr("Title")
        refreshDateLabel()
    }

    Component.onCompleted: {
        initTranslations()
        mousearea_component.createObject(paper)
        mousearea_component.createObject(label_flickable)
    }
}

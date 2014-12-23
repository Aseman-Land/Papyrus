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
import Papyrus 1.0
import AsemanTools 1.0

Item {
    id: paper_label

    property variant paperItem
    property int paperId: paperItem? paperItem.paperItem : -1

    property alias text: txt.text
    property alias font: txt.font
    property alias horizontalAlignment: txt.horizontalAlignment
    property real paintedHeight: txt.visible? txt.paintedHeight : to_do.height
    property real paintedWidth: txt.paintedWidth
    property alias textFocus: txt.focus
    property alias cursorRectangle: txt.cursorRectangle

    property alias pickersVisible: txt.pickersVisible
    property alias pickersPressed: txt.press
    property alias selectedText: txt.selectedText
    property alias commitBlocker: txt.commitBlocker

    property alias selectionStart: txt.selectionStart
    property alias selectionEnd: txt.selectionEnd

    property alias cursorPosition: txt.cursorPosition
    property bool resizeOnFull: true

    property int paperType: Enums.Normal

    onPaperIdChanged: {
        if( paperId != -1 )
            paperType = database.paperType(paperId)
        else
            paperType = Enums.Normal
    }

    onTextChanged: {
        if( to_do.visible )
            to_do.text = text
    }

    onWidthChanged: if(paper_label) paper_label.refreshEditPosition()
    onHeightChanged: if(paper_label) paper_label.refreshEditPosition()
    onXChanged: if(paper_label) paper_label.refreshEditPosition()
    onYChanged: if(paper_label) paper_label.refreshEditPosition()
    onVisibleChanged: if(paper_label) paper_label.refreshEditPosition()

    Connections {
        target: database
        onPaperChanged: {
            if( !paperItem )
                return
            if( paperItem.paperItem != id )
                return

            paperType = database.paperType(paperItem.paperItem)
        }
    }

    Item {
        anchors.fill: parent
    }

    STextEdit{
        id: placeholder_text
        anchors.fill: txt
        font: txt.font
        color: "#cccccc"
        visible: (!txt.focus && txt.text == "" && txt.visible)
    }

    TextAreaCore{
        id: txt
        anchors.fill: parent
        anchors.topMargin: 5*Devices.density
        font.pixelSize: Devices.isLargeTablet? 10*Devices.fontDensity : 9*Devices.fontDensity
        font.family: AsemanApp.globalFontFamily
        wrapMode: Text.Wrap
        smooth: true
        selectionColor: "#0d80ec"
        selectedTextColor: "#ffffff"
        inputMethodHints: globalInputMethodHints
        visible: paperType == Enums.Normal
        Component.onCompleted: highlighter.textDocument = txt.textDocument
    }

    PaperToDo {
        id: to_do
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 5*Devices.density
        visible: paperType == Enums.ToDo
        alignment: txt.horizontalAlignment
        font: txt.font
        color: paperItem? paperItem.groupColor : "#888888"
        onTextChanged: if(visible) paper_label.text = to_do.text
        onVisibleChanged: if(to_do && to_do.visible) to_do.text = paper_label.text
    }

    SearchHighlighter {
        id: highlighter
        searchKeyword: lastSearchKeyword
    }

    Item{
        id: btn_box
        parent: paper
        width: portrait? edit_btn.width + menu_btn.width : edit_btn.width
        height: 30*Devices.density
        z: 20

        Button {
            id: menu_btn
            width: height
            anchors.top: parent.top
            anchors.bottom: btn_box.bottom
            anchors.right: parent.right
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

        Button{
            id: edit_btn
            width: height
            anchors.top: btn_box.top
            anchors.bottom: btn_box.bottom
            anchors.right: menu_btn.left
            anchors.rightMargin: 2*Devices.density
            icon: press? "files/edit_hover_64.png" : "files/edit_64.png"
            iconHeight: 32*Devices.density
            normalColor: "#00000000"
            highlightColor: "#00000000"
            opacity: press? 1 : 0.1
            onClicked: {
                main.focus = true
                paper_label.edit()
            }
        }
    }

    Button {
        id: listview_btn
        parent: paper
        height: 30*Devices.density
        width: height
        icon: press? "files/listview_hover.png" : "files/listview.png"
        iconHeight: 30*Devices.density
        normalColor: "#00000000"
        highlightColor: "#00000000"
        opacity: press? 1 : 0.2
        z: 20
        onClicked: main.showListView()
    }

    function positionToRectangle(pos) {
        return txt.positionToRectangle(pos)
    }

    function isPointOnPickers( x, y ) {
        var pnt = mapToItem(txt,x,y)
        return txt.isPointOnPickers(pnt.x,pnt.y)
    }

    function insertAtCurrent( str ){
        txt.insert( txt.cursorPosition, " " + str + " " )
    }

    function focusOn( x, y ){
        if( txt.visible ) {
            var pnt = mapToItem(txt,x,y)
            txt.focus = true
            txt.cursorPosition = txt.positionAt(pnt.x,pnt.y)
            Devices.showKeyboard()
            showPicker()
        } else {
            var pnt = mapToItem(to_do,x,y)
            to_do.focusOn(pnt.x,pnt.y)
            Devices.showKeyboard()
        }
    }

    function selectWord( x, y ){
        var pnt = mapToItem(txt,x,y)
        focusOn(pnt.x,pnt.y)
        txt.selectWord()
    }

    function edit(){
        main.closePanel()
        var msg = showSubMessage(Qt.createComponent("PaperEditDialog.qml"))
        msg.item = paper_label
        Devices.hideKeyboard()
    }

    function commitFaders() {
        txt.commitFaders()
    }

    function refreshEditPosition(){
        if( !paper )
            return
        btn_box.x = paper_label.mapToItem(paper,paper_label.width,0).x + 14*Devices.density -btn_box.width
        btn_box.y = 15+10*Devices.density
        listview_btn.x = btn_box.x + btn_box.width - listview_btn.width
        listview_btn.y = paper.mapToItem(paper,0,paper.height).y - listview_btn.height - 10*Devices.density - 15
    }

    function showPicker() {
        txt.showPicker()
    }

    function hidePicker() {
        txt.hidePicker()
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        placeholder_text.text = qsTr("New text")
    }

    Component.onCompleted: initTranslations()
}

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
import Papyrus 1.0
import AsemanTools 1.0

Item {
    id: paper_todo
    width: 100
    height: column.height

    property string text
    property int alignment: TextEdit.AlignLeft
    property font font
    property color color

    onTextChanged: {
        if( text == privates.tempText && text.length != 0 )
            return

        paper_todo.refresh()
        privates.tempText = text
    }

    onVisibleChanged: if(paper_todo) paper_todo.refresh()

    QtObject {
        id: privates
        property string tempText
        property bool signalBlocker: false
    }

    ListObject {
        id: list
    }

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: -10*Devices.density
    }

    Component {
        id: item_component
        ToDoItem {
            anchors.left: parent.left
            anchors.right: parent.right
            alignment: paper_todo.alignment
            font: paper_todo.font
            checkColor: paper_todo.color
            visible: paper_todo.visible
        }
    }

    function refreshText() {
        if( privates.signalBlocker )
            return

        var result = ""
        for( var i=0; i<list.count; i++ ) {
            var item = list.at(i)
            if( item.checkable )
                result += item.checked? "* " : "- "

            result += item.text
            if( i < list.count-1 )
                result += "\n"
        }

        privates.tempText = result
        text = result
    }

    function refresh() {
        while( !list.isEmpty() )
            list.takeFirst().destroy()
        if( !visible )
            return

        var lastPos = 0
        var pos = text.indexOf("\n",lastPos)
        while( pos != text.length ) {
            if( pos == -1 )
                pos = text.length

            var str = text.slice(lastPos,pos)
            var slc = str.slice(0,2)
            var checkable = (slc=="* " || slc=="- ")
            if( !checkable && !list.isEmpty() ) {
                privates.signalBlocker = true
                var item = list.last()
                item.text = item.text + "\n" + str
                privates.signalBlocker = false
            } else {
                var item = item_component.createObject(column)
                item.checkable = checkable
                item.text = item.checkable? str.slice(2,str.length) : str
                item.checked = (slc=="* "? true : false)
                item.textChanged.connect(paper_todo.refreshText)
                item.checkedChanged.connect(paper_todo.refreshText)
                item.accepted.connect(paper_todo.createItemAfter)
                item.removed.connect(paper_todo.removeItem)
                item.removeNext.connect(paper_todo.removeNextItem)
                list.append(item)
            }

            lastPos = pos+1
            if( pos != text.length )
                pos = text.indexOf("\n",lastPos)
        }
    }

    function focusOn( x, y ){
        var pnt = paper_todo.mapToItem(column,x,y)
        var item = column.childAt(pnt.x,pnt.y)
        if( !item ) {
            item = list.last()
        }

        var local_pnt = paper_todo.mapToItem(item,x,y)
        item.focusOn(local_pnt.x,local_pnt.y)
    }

    function createItemAfter( item ) {
        var index = list.indexOf(item)
        if( index == -1 )
            return

        var extra_text = item.text.slice(item.cursorPosition,item.text.length)
        item.text = item.text.slice(0,item.cursorPosition)

        var item_new = item_component.createObject(column)
        item_new.checkable = true
        item_new.text = ""
        item_new.checked = false
        item_new.textChanged.connect(paper_todo.refreshText)
        item_new.checkedChanged.connect(paper_todo.refreshText)
        item_new.accepted.connect(paper_todo.createItemAfter)
        item_new.removed.connect(paper_todo.removeItem)
        item_new.removeNext.connect(paper_todo.removeNextItem)
        list.append(item_new)

        privates.signalBlocker = true
        for( var i=list.count-1; i>index+1; i-- ) {
            var item_a = list.at(i)
            var item_b = list.at(i-1)

            item_a.checkable = item_b.checkable
            item_a.text = item_b.text
            item_a.checked = item_b.checked
        }

        var app_item = list.at(index+1)
        app_item.checkable = true
        app_item.text = extra_text
        app_item.checked = false
        app_item.textFocus = true

        privates.signalBlocker = false
        refreshText()
    }

    function removeItem( item ) {
        var index = list.indexOf(item)
        if( index == -1 )
            return
        if( index == 0 )
            return

        var before_item = list.at(index-1)
        var before_pos = before_item.text.length

        privates.signalBlocker = true
        before_item.text = before_item.text + item.text
        before_item.cursorPosition = before_pos
        before_item.textFocus = true
        privates.signalBlocker = false

        list.removeOne(item)
        item.destroy()
        refreshText()
    }

    function removeNextItem( item ) {
        var index = list.indexOf(item)
        if( index == -1 )
            return
        if( index == list.count-1 )
            return

        removeItem( list.at(index+1) )
    }
}

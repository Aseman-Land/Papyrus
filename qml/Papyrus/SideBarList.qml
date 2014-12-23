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

Rectangle {
    id: sidebar_list
    width: 100
    height: 62
    color: "#00000000"

    signal itemSelected( int id, int type )
    signal commandSelected( string cmd )

    property color fontColor: "#333333"

    property variant cacheDate

    Connections{
        target: database
        onGroupsListChanged: refresh()
        onDatesListChanged: refresh()
        onPaperGroupChanged: refresh()
    }

    ListView {
        id: list_view
        anchors.fill: parent
        anchors.topMargin: 20*Devices.density
        anchors.bottomMargin: 20*Devices.density
        highlightMoveDuration: 250
        clip: true
        maximumFlickVelocity: View.flickVelocity

        model: ListModel {}
        delegate: Rectangle {
            id: item
            width: list_view.width
            height: 32*Devices.density
            color: (press && type != -1)? "#3B97EC" : "#00000000"
            radius: 3*Devices.density

            property string text: textValue
            property int iid: guidValue
            property int type: typeValue
            property string command: cmdValue
            property string icon: iconValue

            property bool press: false

            Component.onCompleted:  {
                if( iid == -1 )
                    return
                if( type == 1 ){
                    var append_text = " "
                    var groupCount = database.groupPapersCount(iid)
                    if( papyrus.groupsCount && groupCount != 0 )
                        append_text += "(" + CalendarConv.translateInt(groupCount) + ")"

                    color_rect.color = database.groupColor(iid)
                    text = database.groupName(iid) + append_text
                }
                else
                if( type == 0 ){
                    var days = iid
                    if( days === CalendarConv.currentDays )
                        text = qsTr("Today")
                    else
                    if( days === CalendarConv.currentDays-1 )
                        text = qsTr("Yesterday")
                    else
                    if( days === CalendarConv.currentDays-2 )
                        text = qsTr("Two days ago")
                    else
                        text = CalendarConv.convertIntToStringDate(days,"yyyy MMM dd - dddd")
                }
            }

            Connections{
                target: list_view
                onMovementStarted: press = false
                onFlickStarted: press = false
            }

            Rectangle {
                id: color_rect
                anchors.top: item.top
                anchors.bottom: item.bottom
                anchors.margins: 1*Devices.density
                anchors.left: icon_img.left
                width: 7*Devices.density
                color: "#00000000"
            }

            Image {
                id: icon_img
                height: 22*Devices.density
                width: height
                sourceSize: Qt.size(width,height)
                anchors.left: item.left
                anchors.leftMargin: 6*Devices.density
                anchors.verticalCenter: item.verticalCenter
                source: item.icon
                smooth: true
            }

            Text{
                id: txt
                anchors.left: (icon_img.source != "")? icon_img.right : color_rect.right
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                anchors.leftMargin: 6*Devices.density
                y: parent.height/2 - height/2
                text: parent.text
                font.pixelSize: 9*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                horizontalAlignment: Qt.AlignLeft
                color: item.press? "#ffffff" : fontColor
            }

            MouseArea{
                anchors.fill: parent
                onPressed: item.press = true
                onReleased: item.press = false
                onClicked: {
                    if( item.command === "clean" )
                        main.clean()
                    else
                    if( item.command === "all" ) {
                        main.setCurrentGroup(0, PaperManager.All)
                        show_list_timer.restart()
                    } else
                    if( item.command === "search" )
                        main.showSearch()
                    else
                    if( item.command === "lock" )
                        lock()
                    else
                    if( item.command === "sync" ){
                        if( sync.tokenAvailable ) {
                            sync.refreshForce()
                            syncProgressBar.visible = true
                        }
                    }
                    else
                    if( item.command === "fullscreen" )
                        papyrus.fullscreen = !papyrus.fullscreen
                    else
                    if( item.command === "preferences" )
                        main.showPrefrences()
                    else
                    if( item.command === "history" )
                        main.showHistory()

                    if( item.command !== "" )
                        sidebar_list.commandSelected( item.command )
                    else
                    if( item.type != -1 )
                        sidebar_list.itemSelected( item.iid, (item.type==0)? PaperManager.Date : PaperManager.Group )
                }
            }
        }

        focus: true
        currentIndex: -1

        Component.onCompleted: refresh()
        function refresh(){
            model.clear()

            model.append({"textValue": qsTr("Clean Papers"), "cmdValue": "clean", "typeValue": 0, "guidValue": -1, "iconValue": "files/clean-papers.png"})
            if( papyrus.allPaper )
                model.append({"textValue": qsTr("All Papers"), "cmdValue": "all", "typeValue": 0, "guidValue": -1, "iconValue": "files/all-papers.png"})
            model.append({"textValue": qsTr("Search"), "cmdValue": "search", "typeValue": 0, "guidValue": -1, "iconValue": "files/search.png"})
            if( Devices.isDesktop ) {
                if( database.password().length != 0 )
                    model.append({"textValue": qsTr("Lock"), "cmdValue": "lock", "typeValue": 0, "guidValue": -1, "iconValue": "files/lock.png"})

                model.append({"textValue": qsTr("Fullscreen"), "cmdValue": "fullscreen", "typeValue": 0, "guidValue": -1, "iconValue": "files/fullscreen.png"})
            }
            if( sync.tokenAvailable )
                model.append({"textValue": qsTr("Force Sync"), "cmdValue": "sync", "typeValue": 0, "guidValue": -1, "iconValue": "files/sync.png"})

            model.append({"textValue": qsTr("Preferences"), "cmdValue": "preferences", "typeValue": 0, "guidValue": -1, "iconValue": "files/preferences.png"})

            model.append({"textValue": "", "cmdValue": "", "typeValue": -1, "guidValue": -1, "iconValue": ""})

            model.append({"textValue": qsTr("History"), "cmdValue": "history", "typeValue": 0, "guidValue": -1, "iconValue": "files/history.png"})
            var list = database.lastThreeDays()
            for( var i=0; i<list.length; i++ )
                model.append({"textValue": "", "cmdValue": "", "typeValue": 0, "guidValue": list[i], "iconValue": "files/history.png"})

            model.append({"textValue": "", "cmdValue": "", "typeValue": -1, "guidValue": -1, "iconValue": ""})

            list = database.groups()
            for( var i=0; i<list.length; i++ ) {
                var gid = list[i]
                var name = database.groupName(gid)
                if( database.groupIsDeleted(gid) )
                    continue

                model.append({"textValue": "", "cmdValue": "", "typeValue": 1, "guidValue": gid, "iconValue": ""})
            }
        }
    }

    ScrollBar {
        scrollArea: list_view; height: list_view.height; width: 6*Devices.density
        anchors.right: list_view.right; anchors.top: list_view.top; color: "#ffffff"
    }

    Connections{
        target: papyrus
        onLanguageChanged: sidebar_list.refresh()
        onGroupsCountChanged: sidebar_list.refresh()
        onAllPaperChanged: sidebar_list.refresh()
    }

    Timer {
        id: show_list_timer
        interval: portrait? 1000 : 250
        onTriggered: main.showListView()
    }

    function refresh(){
        list_view.refresh()
    }
}

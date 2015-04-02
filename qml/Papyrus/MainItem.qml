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
    id: main
    width: 48
    height: 70

    property variant preferenceArray: new Array
    property variant dialogItem
    property variant panelItem

    property real panelHeight: 50*Devices.density
    property alias panelWidth: panel.width

    property alias touchToBack: back_area.visible
    property alias darkBackground: dark_background.visible

    property alias sidePanel: panel.visible
    property alias panelAnimDuration: panel.anim_time

    property alias stackSwitcher: stack_switcher

    property variant currentFrame: item_a
    property variant nextFrame: item_b

    property bool menuIsVisible: panel.padX != 0

    onHeightChanged: {
        main_page.anim = false
        if( dialogItem )
            main_page.y = -main.height - View.statusBarHeight - View.navigationBarHeight
    }

    onDialogItemChanged: {
        if( dialogItem )
            BackHandler.pushHandler(main,main.closeDialog)
        else
            BackHandler.removeHandler(main)

        main_page.anim = true
        if( dialogItem )
        {
            dialogItem.parent = dialog_frame
            dialogItem.anchors.fill = dialog_frame
            dialogItem.visible = true

            main_page.panel_anim = false
            main_page.y = -main.height - View.statusBarHeight - View.navigationBarHeight
        }
        else
        {
            main_page.panel_anim = false
            main_page.y = 0
        }
    }

    onPanelItemChanged: {
        if( panelItem )
            BackHandler.pushHandler(main,main.closePanel)
        else
            BackHandler.removeHandler(main)

        main_page.anim = true
        if( panelItem )
        {
            panelHeight = panelItem.height
            panelItem.parent = panel_frame
            panelItem.anchors.fill = panel_frame
            panelItem.visible = true

            main_page.panel_anim = true
            main_page.y = panelHeight
        }
        else
        {
            main_page.panel_anim = true
            main_page.y = 0
        }
    }

    Connections {
        target: AsemanApp
        onBackRequest: BackHandler.back()
    }

    Connections {
        target: papyrus
        onBackRequest: {
            var res = BackHandler.back()
            if( !res && !Devices.isDesktop )
                back_attemper.show()
        }
    }

    Keys.onEscapePressed: BackHandler.back()

    Timer {
        id: back_attemper
        interval: 1500
        repeat: false

        property bool closeCalled: false
        function show() {
            if( closeCalled ) {
                return
            }
            if( back_attemper.running ) {
                closeCalled = true
                papyrus.close()
                return
            }

            back_attemper.start()
            showTooltip( qsTr("Press again to exit") )
        }
    }

    Timer{
        id: panel_visible_timer
        interval: main.panelAnimDuration
        repeat: false
        onTriggered: {
            panelItem = panelItem_tmp
            main.focus = true
        }

        property variant panelItem_tmp
    }

    Image {
        id: background
        x: 0
        y: main_page.y -panelHeight - View.statusBarHeight
        width: main.width
        height: main.height*2.1 + panelHeight + 2*View.navigationBarHeight + 2*View.statusBarHeight
        source: "files/background.jpg"
        fillMode: Image.PreserveAspectCrop
        smooth: false
    }

    Item {
        id: panel_frame
        anchors.left: main_page.left
        width: stack_switcher.width - 20*Devices.density + stack_switcher.platformMargins
        anchors.top: parent.top
        anchors.leftMargin: 10*Devices.density
        anchors.rightMargin: 10*Devices.density
        anchors.topMargin: 2*Devices.density
        height: panelHeight
        z: main_page.y == panelHeight? 10 : 0
    }

    Item {
        id: mirror_back
        anchors.top: parent.top
        anchors.bottom: main_page.bottom
        anchors.topMargin: -View.statusBarHeight
        anchors.bottomMargin: -View.navigationBarHeight
        width: list.width
        x: main_item.parent.x + main_item.width - width
        clip: true
        visible: !portrait && preferenceArray.length==0

        MirrorItem {
            source: background
            anchors.right: parent.right
            anchors.bottom: parent.bottom
        }
    }

    Rectangle {
        anchors.right: mirror_back.right
        anchors.bottom: main_page.top
        width: list.width
        height: 200*Devices.density
        visible: !portrait && preferenceArray.length==0
        color: list.color
        clip: true
    }

    Rectangle {
        anchors.right: mirror_back.right
        anchors.top: main_page.bottom
        anchors.bottom: main_page.bottom
        anchors.bottomMargin: -View.navigationBarHeight
        width: list.width
        visible: !portrait && preferenceArray.length==0
        color: list.color
        clip: true
    }

    Item {
        id: main_page
        x: 0
        y: 0
        width: parent.width
        height: parent.height

        property bool panel_anim: false
        property bool anim: false

        Image{
            id: dark_background
            anchors.fill: main_page
            anchors.topMargin: -View.statusBarHeight
            anchors.bottomMargin: -View.navigationBarHeight
            visible: false
            source: "files/menu_background.jpg"
            fillMode: Image.PreserveAspectCrop
            smooth: false
//            color: "#000000"
        }

        Behavior on y {
            NumberAnimation { easing.type: main_page.panel_anim? Easing.OutCubic : Easing.InOutQuint; duration: main_page.anim? (main_page.panel_anim? 250 : 1000) : 0 }
        }

        PreferenceDialog {
            id: item_a
            Component.onCompleted: item = main_item
        }

        PreferenceDialog {
            id: item_b
        }

        Item{
            id: main_item
            anchors.fill: parent

            SideBarList{
                id: list
                anchors.right: parent.right
                anchors.rightMargin: portrait? 10*Devices.density + Devices.density*40*(1-panel.percent/100) : 0
                width: panel.width
                y: 0
                height: parent.height
                visible: !portrait || panel.visibled || visible_switcher.switcherVariable
                color: portrait? "#00000000" : "#88ffffff"
                fontColor: portrait? "#333333" : "#333333"
                onItemSelected: {
                    panel.itemSelected(id,type)
                    panel.end()
                }
                onCommandSelected: {
                    panel.end()
                }

                Timer{
                    id: visible_switcher
                    repeat: false
                    interval: 2000
                    onTriggered: {
                        if( !switcherVariable ) {
                            switcherVariable = true
                            interval = 100
                            start()
                        } else {
                            switcherVariable = false
                            stop()
                        }
                    }

                    property bool switcherVariable: false
                    Component.onCompleted: visible_switcher.start()
                }
            }

            Item {
                id: stack_switcher_frame
                width: portrait? parent.width : parent.width - list.width
                height: parent.height + View.statusBarHeight + View.navigationBarHeight
                y: -View.statusBarHeight
                anchors.right: portrait? panel.left : list.left
                anchors.rightMargin: portrait? -15 : 0
                clip: true

                PaperStackSwitcher{
                    id: stack_switcher
                    anchors.fill: parent
                    anchors.bottomMargin: View.navigationBarHeight + platformMargins/2
                    anchors.topMargin: View.statusBarHeight + platformMargins/2
                    anchors.leftMargin: platformMargins/2
                    anchors.rightMargin: platformMargins/2

                    DropArea {
                        anchors.fill: parent
                        visible: Devices.isDesktop
                        onDropped: {
                            if( drop.hasUrls ) {
                                var urls = drop.urls
                                for( var i=0; i<urls.length; i++ )
                                    stack_switcher.currentFrame.item.currentPaper.addFile(urls[i])
                            }
                            else
                            if( drop.hasText )
                                stack_switcher.currentFrame.item.currentPaper.bodyText += "\n" + drop.text
                        }
                    }

                    property real platformMargins: Devices.isMobile? -15 + 5*Devices.density : -15 + 5*Devices.density
                }
            }

            SideBar{
                id: panel
                width: portrait? 160*Devices.density : 200*Devices.density
                edge: 15
                pinX: parent.width-edge
                anchors.top: parent.top
                anchors.bottom: parent.bottom
                visible: portrait
                anim_time: portrait? 250 : 10
                onItemSelected: {
                    stack_switcher.show(id, type)
                }

                function refresh(){
                    list.refresh()
                }
            }
        }
    }

    MouseArea{
        id: back_area
        anchors.fill: parent
        visible: false
        onClicked: {
            BackHandler.back()
            Devices.hideKeyboard()
        }
    }

    Item {
        id: dialog_frame
        anchors.left: main_page.left
        anchors.right: main_page.right
        anchors.top: main_page.top
        anchors.topMargin: main.height+View.statusBarHeight+View.navigationBarHeight
        height: main.height
    }

    function refresh(){
        stack_switcher.refresh()
        refreshMenu()
    }

    function refreshMenu(){
        panel.refresh()
    }

    function pushPreference( item ){
        Devices.hideKeyboard()
        var current = (preferenceArray.length == 0)? main_item : preferenceArray[preferenceArray.length-1]
        var next = item

        var res = new Array
        for( var i=0; i<preferenceArray.length; i++ )
            res[res.length] = preferenceArray[i]

        res[res.length] = item
        preferenceArray = res

        currentFrame.anim = false
        currentFrame.x = 0
        currentFrame.opacity = 1
        currentFrame.anim = true
        currentFrame.x = -currentFrame.width
        currentFrame.opacity = (preferenceArray.length == 1)? 1 : 0

        nextFrame.anim = false
        nextFrame.x = nextFrame.width
        nextFrame.opacity = (preferenceArray.length == 1)? 1 : 0
        nextFrame.item = next
        nextFrame.anim = true
        nextFrame.x = 0
        nextFrame.opacity = 1

        var tmp = nextFrame
        nextFrame = currentFrame
        currentFrame = tmp

        main_page.focus = true

        if( preferenceArray.length == 1 )
            BackHandler.pushHandler(main,main.popPreference)
    }

    function popPreference(){
        Devices.hideKeyboard()
        var current = preferenceArray[preferenceArray.length-1]
        var res = new Array
        for( var i=0; i<preferenceArray.length-1; i++ )
            res[res.length] = preferenceArray[i]

        preferenceArray = res

        var next = (preferenceArray.length == 0)? main_item : preferenceArray[preferenceArray.length-1]

        currentFrame.anim = false
        currentFrame.x = 0
        currentFrame.opacity = 1
        currentFrame.anim = true
        currentFrame.x = currentFrame.width
        currentFrame.opacity = (preferenceArray.length == 0)? 1 : 0

        nextFrame.anim = false
        nextFrame.x = -nextFrame.width
        nextFrame.opacity = (preferenceArray.length == 0)? 1 : 0
        nextFrame.item = next
        nextFrame.anim = true
        nextFrame.x = 0
        nextFrame.opacity = 1

        if( current !== main_item )
            Tools.deleteItemDelay( current, 250 )

        var tmp = nextFrame
        nextFrame = currentFrame
        currentFrame = tmp

        main.focus = true

        if( preferenceArray.length == 0 )
            BackHandler.removeHandler(main)
        else
            return false
    }

    function showDialog( item ){
        if( dialogItem )
            dialogItem.destroy()

        Devices.hideKeyboard()
        dialogItem = item
        main.focus = true
    }

    function closeDialog(){
        if( !dialogItem )
            return

        main_page.anim = true
        Devices.hideKeyboard()
        main_page.y = 0
        Tools.deleteItemDelay(dialogItem,1000)
        main.focus = true
    }

    function showPanel( item ){
        if( !item && panelItem ) {
            closePanel()
            panelItem.destroy()
            return
        }
        if( panelItem )
            panelItem.destroy()

        item.visible = false
        panel_visible_timer.panelItem_tmp = item
        panel_visible_timer.start()
    }

    function showPanelImedietly( item ){
        if( !item && panelItem ) {
            closePanel()
            panelItem.destroy()
            return
        }
        if( panelItem )
            panelItem.destroy()

        item.visible = false
        panelItem = item
        main.focus = true
    }

    function closePanel(){
        if( !panelItem )
            return

        main_page.anim = true
        back_area.visible = false
        main_page.y = 0
        Tools.deleteItemDelay(panelItem,250)
        main.focus = true
    }

    function setCurrentGroup( id, type ){
        stack_switcher.show(id,type)
    }

    function search( txt ){
        stack_switcher.keyword = txt
        stack_switcher.show(0,PaperManager.Search)
    }

    function advanceSearch( keyword, startDate, endDate, startTime, endTime, group, domain, geo, weather ) {
        stack_switcher.keyword = keyword
        stack_switcher.startDate = startDate
        stack_switcher.endDate = endDate
        stack_switcher.startTime = startTime
        stack_switcher.endTime = endTime
        stack_switcher.selectedGid = group
        stack_switcher.paperType = domain
        stack_switcher.geo = geo
        stack_switcher.weather = weather
        stack_switcher.show(0,PaperManager.AdvanceSearch)
    }

    function showSearch(){
        var item = search_panel_component.createObject(panel_frame);
        showPanel(item)
    }

    function clean(){
        stack_switcher.clean()
    }

    function showPrefrences(){
        var item = preference_component.createObject(main);
        main.pushPreference(item)
    }

    function showHistory(){
        var item = history_component.createObject(main);
        main.pushPreference(item)
    }

    function showListView(){
        var list = stack_switcher.currentFrame.item.papers
        if( !list || list.length == 0 ) {
            showTooltip(qsTr("List is empty"))
            return
        }

        var item = paperlist_component.createObject(main);
        main.pushPreference(item)
    }

    function showMenu(){
        panel.showPanel()
    }

    function incomingShare( title, message ) {
        stack_switcher.incomingShare(title,message)
    }

    function incomingImage( path ) {
        stack_switcher.incomingImage(path)
    }

    function initPapers() {
        stack_switcher.init()
    }

    Component {
        id: history_component
        History {}
    }

    Component {
        id: preference_component
        Preference {}
    }

    Component {
        id: paperlist_component
        PaperList {
            list: {
                var list = stack_switcher.currentFrame.item.papers
                if( !list || list.length == 0 ) {
                    showTooltip(qsTr("List is empty"))
                    return new Array
                } else {
                    return list
                }
            }
            Component.onCompleted: {
                positionViewAtIndex(main.stackSwitcher.currentFrame.item.currentIndex)
            }
        }
    }

    Component {
        id: search_panel_component
        SearchPanel {
            onKeywordChanged: main.search(text)
            onAdvanceSearchRequest: main.advanceSearch(keyword, startDate, endDate, startTime, endTime, group, domain, geo, weather)
        }
    }
}

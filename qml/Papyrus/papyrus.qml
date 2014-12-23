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
import QtMultimedia 5.0
import QtPositioning 5.2
import AsemanTools 1.0
import Papyrus 1.0

AsemanMain {
    id: papyrus_root
    width: Devices.isTouchDevice? 0 : papyrus.size.width
    height: Devices.isTouchDevice? 0 : papyrus.size.height
    mainFrame: main_frame
    panelWidth: mainItem? mainItem.panelWidth : width
    subMessageBlur: Devices.isDesktop
    subMessageBackground: Devices.isDesktop? "#66ffffff" : "#e5ffffff"

    property real fixedHeight: height-View.statusBarHeight-View.navigationBarHeight

    property int globalInputMethodHints: papyrus.keyboardPredicative? Qt.ImhNone : Qt.ImhNoPredictiveText

    property bool rotated: true
    property bool inited: false

    property alias mainItem: main
    property alias syncProgressBar: sync_pbar

    property alias position: positioning.position
    property alias weather: weather_model.weather
    property alias audioItem: audio_item

    property bool passDone: false

    property int publicCounter: 0
    property string lastSearchKeyword

    signal accessChanged()

    onHeightChanged: {
        size_save_timer.restart()
    }
    onWidthChanged: {
        size_save_timer.restart()
    }

    onLastSearchKeywordChanged: if(lastSearchKeyword.length==0) hideBottomPanel()

    Connections {
        target: database
        onPaperChanged: {
            if( !papyrus.demoHasTrial() )
                showSubMessage(Qt.createComponent("DemoLimited.qml"))
        }
    }

    Timer {
        id: size_save_timer
        interval: 300
        repeat: false
        onTriggered: papyrus.size = Qt.size(width,height)
    }

    FontLoader{
        id: papyrus_normal_font
        source: papyrus.resourcePath + "/fonts/DroidKaqazSans.ttf"
        onStatusChanged: if(status == FontLoader.Ready) AsemanApp.globalFontFamily = name
    }

    FontLoader{
        id: papyrus_mono_font
        source: papyrus.resourcePath + "/fonts/DroidSansMono.ttf"
        onStatusChanged: if(status == FontLoader.Ready) AsemanApp.globalMonoFontFamily = name
    }

    Item {
        id: main_frame
        anchors.fill: parent

        MainItem {
            id: main
            x: 0
            y: View.statusBarHeight
            width: parent.width
            height: parent.height - View.statusBarHeight - View.navigationBarHeight
        }
    }

    Audio {
        id: audio_item

        property variant activeBy
        onActiveByChanged: if( !activeBy ) audio_item.stop()
    }

    WeatherModel {
        id: weather_model
        active: inited && papyrus.positioning && papyrus.weatherActive && papyrus.proBuild
        geoCoordinate: position.coordinate
    }

    PositionSource {
        id: positioning
        updateInterval: 10000
        active: inited && papyrus.positioning
    }

    ProgressBar {
        id: sync_pbar
        height: 6*Devices.density
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.rightMargin: portrait? 0 : panelWidth
        color: "#aaffffff"
        radius: 0
        visible: false
        onVisibleChanged: percent = 0

        Connections {
            target: sync
            onSyncProgress: sync_pbar.percent = val*100
            onSyncStarted: {
                invisible_timer.stop()
                sync_pbar.visible = true
                sync_pbar.percent = 0
            }
            onSyncFinished: {
                invisible_timer.restart()
                sync_pbar.percent = 100
            }
        }

        Timer {
            id: invisible_timer
            interval: 2000
            repeat: false
            onTriggered: sync_pbar.visible = false
        }
    }

    function showHideHilightPanel() {
        var component = Qt.createComponent("SearchHideHighlights.qml")
        showBottomPanel( component, portrait )
    }

    function showHistory(){
        mainItem.showHistory()
    }

    function createPaper( parent ){
        var paper = paper_cmpnt.createObject(parent);
        return paper
    }

    function setCurrentGroup( id ){
        mainItem.setCurrentGroup(id)
    }

    function getPapyrus(){
        return papyrus
    }

    function refresh(){
        sync_pbar.visible = false
        mainItem.refresh()
        sync.reload()
    }

    function clean(){
        mainItem.clean()
    }

    function showHelper(){
        var component = Qt.createComponent("Helper.qml")
        var item = component.createObject(papyrus_root);
        return item
    }

    function showSplash(){
        var component = Qt.createComponent("StartSplash.qml")
        return showSubMessage(component)
    }

    function search(){
        mainItem.showSearch()
    }

    function prefrences(){
        mainItem.showPrefrences()
    }

    function getPass( parent ){

        var blur_item
        if( Devices.isDesktop ) {
            var blur_cmpt = Qt.createComponent("PapyrusBlur.qml");
            blur_item = blur_cmpt.createObject(papyrus_root);
            blur_item.source = mainFrame
        }

        var item = get_pass_cmpnt.createObject(papyrus_root);
        item.password = database.password()
        item.backBlur = blur_item
        item.parentItem = parent

        if( blur_item )
            item.success.connect(blur_item.destroy)

        return item
    }

    function accessGranted(){
        passDone = true
        papyrus_root.accessChanged()
    }

    function accessDenied(){
        passDone = false
        papyrus_root.accessChanged()
    }

    function lock(){
        if( database.password().length === 0 )
            return

        accessDenied()
        getPass(papyrus_root).success.connect(papyrus_root.accessGranted)
    }

    function showMenu(){
        mainItem.showMenu()
    }

    function incomingShare( title, message ) {
        mainItem.incomingShare(title,message)
    }

    function incomingImage( path ) {
        mainItem.incomingImage(path)
    }

    Component {
        id: get_pass_cmpnt
        GetPassDialog {
            password: database.password()
            inputMethodHints: database.passwordType()==0? Qt.ImhDigitsOnly : Qt.ImhNone
        }
    }

    Component {
        id: paper_cmpnt
        PaperItem {
        }
    }

    Timer {
        id: init_timer
        interval: 1000
        onTriggered: {
            inited = true
        }
    }

    Timer {
        id: main_init_timer
        interval: 20
        onTriggered: {
            main.initPapers()
            init_timer.restart()
        }
    }

    Component.onCompleted: {
        if( database.password().length !== 0 )
            lock()
        else
            passDone = true
        if( !papyrus.isTutorialCompleted() )
            showSplash()
        else
        if( !papyrus.demoHasTrial() )
            showSubMessage(Qt.createComponent("DemoLimited.qml"))

        main_init_timer.restart()
    }
}

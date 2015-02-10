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
    id: search_panel
    width: 100
    height: 50*Devices.density

    property alias text: txt.text

    signal keywordChanged( string text )
    signal advanceSearchRequest( string keyword, variant startDate, variant endDate, variant startTime, variant endTime, int group, int domain, variant geo, string weather )

    onTextChanged: search_timer.restart()

    QtObject {
        id: privates
        property variant moreDialog

        property date startDate
        property date endDate
        property bool dateIsSet: false

        property date startTime
        property date endTime
        property bool timeIsSet: false

        property string domainSelectedText
        property int paperType: Enums.AllPapers
        property int selectedGid: -1

        property string weather
        property variant geo

        onMoreDialogChanged: more_btn.more = moreDialog? true : false
    }

    Rectangle{
        id: back
        color: "#66ffffff"
        anchors.fill: parent
        anchors.margins: 20*Devices.density
        anchors.topMargin: 10*Devices.density
        anchors.bottomMargin: 0
        radius: 3
    }

    Image {
        id: icon
        anchors.top: back.top
        anchors.bottom: back.bottom
        anchors.left: back.left
        anchors.margins: 10*Devices.density
        width: height
        source: "files/search.png"
    }

    Text{
        id: placeholder_txt
        anchors.verticalCenter: back.verticalCenter
        anchors.left: icon.right
        anchors.right: back.right
        anchors.margins: 10*Devices.density
        color: "#ffffff"
        visible: (!txt.focus && txt.text == "")
        font: txt.font
    }

    STextInput{
        id: txt
        anchors.verticalCenter: back.verticalCenter
        anchors.left: icon.right
        anchors.right: more_btn.left
        anchors.margins: 10*Devices.density
        font.pixelSize: 13*Devices.fontDensity
        font.family: AsemanApp.globalFont.family
        inputMethodHints: Qt.ImhNoPredictiveText
    }

    Button {
        id: more_btn
        anchors.topMargin: 2*Devices.density
        anchors.top: back.top
        anchors.bottom: back.bottom
        anchors.right: back.right
        width: 60*Devices.density
        text: more? qsTr("Less") : qsTr("More")
        textFont.pixelSize: 10*Devices.fontDensity
        textFont.family: AsemanApp.globalFont.family
        textColor: press? "#0d80ec" : "#333333"
        highlightColor: "#00000000"
        onClicked: {
            more = !more
            Devices.hideKeyboard()
            if( more )
                privates.moreDialog = showBottomPanel(more_dialog_component,portrait)
            else
                hideBottomPanel()
        }

        property bool more: false
    }

    Timer{
        id: active_focus_timer
        interval: 501
        repeat: false
        onTriggered: txt.focus = true
    }

    Timer{
        id: search_timer
        interval: 700
        repeat: false
        onTriggered: startSearch()
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    Component {
        id: more_dialog_component
        SearchMorePanel {
            onDateIsSetChanged: { privates.dateIsSet = dateIsSet; refreshSearch() }
            onStartDateChanged: { privates.startDate = startDate; refreshSearch() }
            onEndDateChanged: { privates.endDate = endDate; refreshSearch() }
            onTimeIsSetChanged: { privates.timeIsSet = timeIsSet; refreshSearch() }
            onStartTimeChanged: { privates.startTime = startTime; refreshSearch() }
            onEndTimeChanged: { privates.endTime = endTime; refreshSearch() }
            onPaperTypeChanged: { privates.paperType = paperType; refreshSearch() }
            onSelectedGidChanged: { privates.selectedGid = selectedGid; refreshSearch() }
            onDomainSelectedTextChanged: { privates.domainSelectedText = domainSelectedText; refreshSearch() }
            onGeoChanged: { privates.geo = geo; refreshSearch() }
            onWeatherChanged: { privates.weather = weather; refreshSearch() }

            Component.onCompleted: {
                dateIsSet = privates.dateIsSet
                startDate = privates.startDate
                endDate = privates.endDate
                timeIsSet = privates.timeIsSet
                startTime = privates.startTime
                endTime = privates.endTime
                paperType = privates.paperType
                selectedGid = privates.selectedGid
                domainSelectedText = privates.domainSelectedText
                geo = privates.geo
                weather = privates.weather
            }
        }
    }

    function initTranslations(){
        placeholder_txt.text = qsTr("Search keyword")
    }

    function startSearch() {
        if( privates.dateIsSet || privates.timeIsSet || privates.paperType!=Enums.AllPapers || privates.selectedGid!=-1 || privates.geo || privates.weather.length != 0 ) {
            search_panel.advanceSearchRequest(text, privates.dateIsSet? privates.startDate : 0,
                                                    privates.dateIsSet? privates.endDate : 0,
                                                    privates.timeIsSet? privates.startTime : 0,
                                                    privates.timeIsSet? privates.endTime : 0,
                                                    privates.selectedGid, privates.paperType,
                                                    privates.geo, privates.weather )
        } else {
            search_panel.keywordChanged(text)
        }
    }

    function refreshSearch() {
        search_timer.restart()
    }

    Component.onCompleted: {
        active_focus_timer.start()
        main.touchToBack = true
        lastSearchKeyword = ""
        initTranslations()
    }

    Component.onDestruction: {
        if( lastSearchKeyword.length != 0 ) {
            showHideHilightPanel()
        }
    }
}

/*
    Copyright (C) 2014 Aseman
    http://aseman.co

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
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
    id: more_panel
    width: 100
    height: flickable.height + header_frame.height

    property bool startPage: true

    property date startDate
    property date endDate
    property bool dateIsSet: false

    property date startTime
    property date endTime
    property bool timeIsSet: false

    property variant geo
    property string weather

    property string domainSelectedText
    property int paperType: Enums.AllPapers
    property int selectedGid: -1

    onStartPageChanged: {
        if( startPage )
            BackHandler.removeHandler(more_panel)
        else
            BackHandler.pushHandler(more_panel,more_panel.back)
    }

    MouseArea {
        anchors.fill: parent
    }

    Item {
        id: header_frame
        height: 50*Devices.density
        width: parent.width

        Button {
            id: back_btn
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            anchors.leftMargin: 10*Devices.density
            height: 30*Devices.density
            width: 100*Devices.density
            color: "#ff5532"
            highlightColor: "#E34B2C"
            textColor: "#ffffff"
            text: qsTr("Back")
            visible: !startPage
            onClicked: {
                more_panel.startPage = true
                item_destroy_timer.restart()
            }
        }

        Button {
            id: done_btn
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 10*Devices.density
            height: 30*Devices.density
            width: 100*Devices.density
            color: "#4098bf"
            highlightColor: "#3B8DB1"
            textColor: "#ffffff"
            text: qsTr("Done")
            onClicked: {
                if( !startPage ) {
                    more_panel.startPage = true
                    item_destroy_timer.restart()
                    if( pages_frame.item )
                        pages_frame.item.done()
                } else {
                    hideBottomPanel()
                }
            }
        }
    }

    Flickable {
        id: flickable
        anchors.top: header_frame.bottom
        x: startPage? 0 : -width
        height: 220*Devices.density
        width: parent.width
        contentWidth: column.width
        contentHeight: column.height
        flickableDirection: Flickable.VerticalFlick
        clip: true

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 400 }
        }

        Column {
            id: column
            width: flickable.width

            MenuButton {
                id: date_dmn_btn
                height: 50*Devices.density
                width: column.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Date Domain")
                onClicked: {
                    showItem(date_domain_component)
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    x: date_dmn_btn.textAlignment==Text.AlignRight? 10*Devices.density : parent.width-width-10*Devices.density
                    spacing: -4*Devices.density

                    Text {
                        id: start_date_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.dateIsSet? "<b>From:</b> " + CalendarConv.convertIntToStringDate(CalendarConv.convertDateToDays(more_panel.startDate)) : ""
                    }

                    Text {
                        id: end_date_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.dateIsSet? "<b>To:</b> " + CalendarConv.convertIntToStringDate(CalendarConv.convertDateToDays(more_panel.endDate)) : ""
                    }
                }
            }

            MenuButton {
                id: time_dmn_btn
                height: 50*Devices.density
                width: column.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Time Domain")
                onClicked: {
                    showItem(time_domain_component)
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    x: time_dmn_btn.textAlignment==Text.AlignRight? 10*Devices.density : parent.width-width-10*Devices.density
                    spacing: -4*Devices.density

                    Text {
                        id: start_time_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.timeIsSet? "<b>From:</b> " + more_panel.startTime.toLocaleTimeString() : ""
                    }

                    Text {
                        id: end_time_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.timeIsSet? "<b>To:</b> " + more_panel.endTime.toLocaleTimeString() : ""
                    }
                }
            }

            MenuButton {
                id: geo_select_btn
                height: 50*Devices.density
                width: column.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Geo Location Domain")
                onClicked: {
                    showItem(geo_domain_component)
                }

                Text {
                    id: geo_text
                    anchors.verticalCenter: parent.verticalCenter
                    x: geo_select_btn.textAlignment==Text.AlignRight? 10*Devices.density : parent.width-width-10*Devices.density
                    font.pixelSize: 8*Devices.fontDensity
                    font.family: AsemanApp.globalFont.family
                    color: "#333333"
                    text: more_panel.geo? littleNum(more_panel.geo.y) + ", " + littleNum(more_panel.geo.x) + "  " +
                                          littleNum(more_panel.geo.width) + "x" + littleNum(more_panel.geo.height) : ""

                    function littleNum( num ) {
                        return Math.floor(num*1000)/1000
                    }
                }
            }

            MenuButton {
                id: weather_btn
                height: 50*Devices.density
                width: column.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Weather")
                visible: papyrus.proBuild
                onClicked: {
                    showItem(weather_component)
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    x: weather_btn.textAlignment==Text.AlignRight? 10*Devices.density : parent.width-width-10*Devices.density
                    spacing: -4*Devices.density

                    Text {
                        id: weather_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.weather
                    }
                }
            }

            MenuButton {
                id: group_btn
                height: 50*Devices.density
                width: column.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Labels")
                onClicked: {
                    showItem(group_domain_component)
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    x: group_btn.textAlignment==Text.AlignRight? 10*Devices.density : parent.width-width-10*Devices.density
                    spacing: -4*Devices.density

                    Text {
                        id: group_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.selectedGid!=-1? database.groupName(more_panel.selectedGid) : ""
                    }
                }
            }

            MenuButton {
                id: paper_type_btn
                height: 50*Devices.density
                width: column.width
                normalColor: "#00000000"
                highlightColor: "#4098bf"
                textColor: press? "#ffffff" : "#4098bf"
                textFont.weight: Font.Normal
                textFont.pixelSize: 13*Devices.fontDensity
                textFont.bold: false
                text: qsTr("Paper Type")
                onClicked: {
                    showItem(type_domain_component)
                }

                Column {
                    anchors.verticalCenter: parent.verticalCenter
                    x: paper_type_btn.textAlignment==Text.AlignRight? 10*Devices.density : parent.width-width-10*Devices.density
                    spacing: -4*Devices.density

                    Text {
                        id: paper_type_text
                        font.pixelSize: 8*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        text: more_panel.domainSelectedText
                    }
                }
            }
        }
    }

    ScrollBar {
        scrollArea: flickable; height: flickable.height; width: 6*Devices.density
        anchors.right: flickable.right; anchors.top: flickable.top; color: "#000000"
    }

    Item {
        id: pages_frame
        anchors.top: flickable.top
        anchors.left: flickable.right
        anchors.bottom: flickable.bottom
        width: parent.width
        clip: true

        property variant item

        onItemChanged: {
            if( item )
                item.anchors.fill = pages_frame
        }

        Timer {
            id: item_destroy_timer
            interval: 400
            onTriggered: if(pages_frame.item) pages_frame.item.destroy()
        }
    }

    Component {
        id: date_domain_component
        SearchDateDomain {
            id: date_domain_item
            height: pages_frame.height

            function done() {
                more_panel.startDate = startDate.getDate()
                more_panel.endDate = endDate.getDate()
                more_panel.dateIsSet = true
            }
        }
    }

    Component {
        id: time_domain_component
        SearchTimeDomain {
            id: time_domain_item
            height: pages_frame.height

            function done() {
                more_panel.startTime = startTime.getDate()
                more_panel.endTime = endTime.getDate()
                more_panel.timeIsSet = true
            }
        }
    }

    Component {
        id: group_domain_component
        SearchGroupDomain {
            id: group_domain_item
            height: pages_frame.height
            onSelectedGidChanged: {
                more_panel.startPage = true
                item_destroy_timer.restart()
                done()
            }

            function done() {
                more_panel.selectedGid = selectedGid
            }
        }
    }

    Component {
        id: geo_domain_component
        SearchGeoDomain {
            id: geo_domain_item
            height: pages_frame.height

            function done() {
                more_panel.geo = domain
            }
        }
    }

    Component {
        id: type_domain_component
        SearchPaperTypeDomain {
            id: type_domain_item
            height: pages_frame.height
            onDomainChanged: {
                more_panel.startPage = true
                item_destroy_timer.restart()
                done()
            }

            function done() {
                more_panel.domainSelectedText = selectedText
                more_panel.paperType = domain
            }
        }
    }

    Component {
        id: weather_component
        SearchPaperWeather {
            height: pages_frame.height

            onWeatherChanged: {
                more_panel.startPage = true
                item_destroy_timer.restart()
                done()
            }

            function done() {
                more_panel.weather = weather
            }
        }
    }

    function showItem( component ) {
        pages_frame.item = component.createObject(pages_frame)
        startPage = false
    }

    function back(){
        if( startPage )
            return true

        more_panel.startPage = true
        item_destroy_timer.restart()
        return false
    }
}

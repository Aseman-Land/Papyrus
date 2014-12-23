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
import AsemanTools 1.0

PaperAbstractButton {
    id: pt_btn
    background: "files/paperbuttons/temperature.png"

    property int paperTemperature: paperItem>0? database.paperTemperature(paperItem) : 0
    property variant temperatureChooser

    onClicked: {
        if( !temperatureChooser )
            temperatureChooser = showBottomPanel(paper_temperature_component,true)
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.bottomMargin: 3*Devices.density
        font.pixelSize: 30*Devices.fontDensity
        font.family: AsemanApp.globalFontFamily
        font.bold: true
        color: "#ffffff"
        text: paperTemperature + "°"
    }

    Component {
        id: paper_temperature_component

        Column {
            width: 20
            spacing: 4*Devices.density

            Item { width: 20; height: 10*Devices.density }

            Button {
                id: done_btn
                anchors.right: parent.right
                anchors.margins: 10*Devices.density
                height: 30*Devices.density
                width: 100*Devices.density
                color: "#4098bf"
                highlightColor: "#3B8DB1"
                textColor: "#ffffff"
                text: qsTr("Done")
                onClicked: {
                    var temp = Math.floor((slider.value-50)*2)
                    database.setPaperTemperature(paperItem,temp)
                    main.refreshMenu()
                    hideBottomPanel()
                }
            }

            Row {
                anchors.horizontalCenter: parent.horizontalCenter
                spacing: 10*Devices.density

                Slider {
                    id: slider
                    anchors.verticalCenter: parent.verticalCenter
                    width: 220*Devices.density
                    height: 25*Devices.density
                    maximum: 100
                    value: 50 + paperTemperature/2
                    backColor: "#333333"
                }

                Text {
                    width: 30*Devices.density
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.margins: 10*Devices.density
                    font.family: AsemanApp.globalFontFamily
                    font.pixelSize: 20*Devices.fontDensity
                    color: "#333333"
                    text: Math.floor((slider.value-50)*2) + "°"
                }
            }
        }
    }
}

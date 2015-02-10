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

PaperAbstractButton {
    id: pw_btn
    background: "files/weather/weather-unknown.png"

    property string paperWeather: paperItem>0? database.paperWeather(paperItem) : ""
    property variant paperDate: paperItem>0? database.paperCreatedDate(paperItem) : 0

    property variant weatherChooser

    property bool night: false

    onClicked: {
        if( !weatherChooser )
            weatherChooser = showBottomPanel(paper_weather_component,true)
    }

    onRefresh: {
        var pw = paperWeather
        if( pw.length == 0 )
            pw = "weather-unknown"
        if( night && (pw == "weather-clear" || pw == "weather-clouds") )
            pw = pw + "-night"

        background = "files/weather/" + pw + ".png"
    }

    onPaperWeatherChanged: refresh()
    onNightChanged: refresh()

    onPaperDateChanged: {
        if( !paperDate )
            return

        if( paperDate.getHours()<12 )
            night = false
        else
            night = true
    }

    Component {
        id: paper_weather_component

        Column {
            width: 20

            Text {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10*Devices.density
                font.family: AsemanApp.globalFont.family
                font.pixelSize: 12*Devices.fontDensity
                color: "#333333"
                text: qsTr("Please select weather")
            }

            ListView {
                id: pw_list
                height: 92*Devices.density
                width: parent.width
                orientation: ListView.Horizontal
                model: ListModel{}
                delegate: Item {
                    width: height
                    height: pw_list.height

                    Image {
                        anchors.fill: parent
                        asynchronous: true
                        fillMode: Image.PreserveAspectFit
                        sourceSize: Qt.size(width,height)
                        source: "files/weather/" + iconId + ".png"
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            paperObject.save()
                            database.setPaperWeather(paperItem,iconId)
                            pw_btn.refresh()
                            hideBottomPanel()
                        }
                    }
                }

                Component.onCompleted: {
                    if( night ) {
                        pw_list.model.append( {"iconId": "weather-clear-night"} )
                        pw_list.model.append( {"iconId": "weather-clouds-night"} )
                    } else {
                        pw_list.model.append( {"iconId": "weather-clear"} )
                        pw_list.model.append( {"iconId": "weather-clouds"} )
                    }

                    pw_list.model.append( {"iconId": "weather-overcast"} )
                    pw_list.model.append( {"iconId": "weather-showers"} )
                    pw_list.model.append( {"iconId": "weather-storm"} )
                    pw_list.model.append( {"iconId": "weather-snow"} )
                    pw_list.model.append( {"iconId": "weather-fog"} )
                    pw_list.model.append( {"iconId": "weather-unknown"} )
                }
            }
        }
    }
}

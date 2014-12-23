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
import QtPositioning 5.0

PaperAbstractButton {
    background: "files/paperbuttons/map.png"

    property variant paperLocation: paperItem>0? database.paperLocation(paperItem) : 0

    onClicked: {
        showBottomPanel(geo_component,true)
    }

    Component {
        id: geo_component
        Item {
            id: geo_dialog
            height: 300*Devices.density

            MouseArea {
                anchors.fill: parent
            }

            Button {
                id: back_btn
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.margins: 10*Devices.density
                height: 30*Devices.density
                width: 100*Devices.density
                color: "#ff5532"
                highlightColor: "#E34B2C"
                textColor: "#ffffff"
                text: qsTr("Unpin")
                onClicked: {
                    database.setPaperLocation(paperItem,QtPositioning.coordinate(0,0))
                    hideBottomPanel()
                }
            }

            Button {
                id: done_btn
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 10*Devices.density
                height: 30*Devices.density
                width: 100*Devices.density
                color: "#4098bf"
                highlightColor: "#3B8DB1"
                textColor: "#ffffff"
                text: qsTr("Done")
                onClicked: {
                    var geo = map_switcher.view
                    database.setPaperLocation(paperItem,geo)
                    main.refreshMenu()
                    hideBottomPanel()
                }
            }

            MapSwitcher {
                id: map_switcher
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: done_btn.bottom
                anchors.topMargin: 10*Devices.density
                Component.onCompleted: {
                    if( paperLocation != 0 && (paperLocation.longitude != 0 || paperLocation.latitude != 0) )
                        view = Qt.point(paperLocation.longitude,paperLocation.latitude)

                    console.debug(paperLocation.longitude, paperLocation.latitude, paperLocation)
                    crosshairs = true
                }
            }
        }
    }
}

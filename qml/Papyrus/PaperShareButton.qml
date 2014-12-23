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
    id: pshare_btn

    onClicked: {
        if( Devices.isLinux && !Devices.isAndroid ) {
            hideSubMessage()
            var path = papyrus.getStaticTempPath()
            papyrus.shareToFile( database.paperTitle(paperItem),
                               database.paperText(paperItem),
                               path )

            var msg = showSubMessage(Qt.createComponent("PapyrusShareDialog.qml"))
            msg.sources = [path]
        } else {
            Devices.share( database.paperTitle(paperItem),
                         database.paperText(paperItem) )
        }
    }

    Rectangle {
        anchors.fill: parent
        color: pressed? "#3580A1" : "#3B8DB1"
        radius: 4*Devices.density
        anchors.margins: 5*Devices.density

        Text {
            anchors.centerIn: parent
            font.pixelSize: 12*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#ffffff"
            text: qsTr("Share Paper")
        }
    }
}

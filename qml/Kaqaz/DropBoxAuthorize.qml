/*
    Copyright (C) 2014 Sialan Labs
    http://labs.sialan.org

    Kaqaz is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Kaqaz is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import SialanTools 1.0

Item {
    id: dbox_auth
    anchors.fill: parent

    property bool started: false

    Connections {
        target: sync
        onAuthorizeRequest: authorize_timer.restart()
    }

    Timer {
        id: authorize_timer
        interval: 1000
        repeat: false
        onTriggered: {
            indicator.stop()
            connecting.visible = false
            btn.visible = true
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton | Qt.LeftButton
        hoverEnabled: true
        onWheel: wheel.accepted = true
    }

    Header {
        id: header
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        z: 10
    }

    Item {
        anchors.fill: parent
        anchors.topMargin: View.statusBarHeight
        anchors.bottomMargin: View.navigationBarHeight

        Column {
            id: column
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter

            Image {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 256*physicalPlatformScale
                height: width
                sourceSize: Qt.size(width,height)
                source: "files/dropbox.png"
                fillMode: Image.PreserveAspectFit
            }

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                font.pixelSize: 20*fontsScale
                font.family: SApp.globalFontFamily
                color: "#007ee5"
                text: "Dropbox Authorize"
            }
        }

        Indicator {
            id: indicator
            anchors.top: column.bottom
            anchors.bottom: btn.top
            anchors.left: parent.left
            anchors.right: parent.right
            indicatorSize: 22*physicalPlatformScale
            Component.onCompleted: indicator.start()
        }

        Text {
            id: connecting
            anchors.horizontalCenter: indicator.horizontalCenter
            anchors.top: indicator.bottom
            anchors.topMargin: 8*physicalPlatformScale
            font.pixelSize: 10*fontsScale
            font.family: SApp.globalFontFamily
            color: "#555555"
            text: qsTr("Connecting...")
        }

        LineEdit {
            id: password_line
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: password_repeat.top
            anchors.margins: 12*physicalPlatformScale
            visible: btn.visible
            echoMode: TextInput.Password
            placeholder: qsTr("Password (optional and unchangable)")
            onTextChanged: password_repeat.text = ""
            onAccepted: password_repeat.lineFocus = true
        }

        LineEdit {
            id: password_repeat
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: btn.top
            anchors.margins: 12*physicalPlatformScale
            visible: btn.visible
            echoMode: TextInput.Password
            placeholder: qsTr("Password Repeat")
            onAccepted: done()
            onTextChanged: {
                if( text == password_line.text ) {
                    password_repeat.border.width = 1*physicalPlatformScale
                    password_repeat.border.color = "#8800ff00"
                } else {
                    password_repeat.border.width = 1*physicalPlatformScale
                    password_repeat.border.color = "#aaff0000"
                }
            }
        }

        Button {
            id: btn
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.margins: 12*physicalPlatformScale
            height: 42*physicalPlatformScale
            text: qsTr("Finished")
            highlightColor: "#880d80ec"
            normalColor: "#88ffffff"
            textColor: "#333333"
            visible: false
            onClicked: done()
        }
    }

    function done() {
        if( password_repeat.text != password_line.text )
            return

        sync.setPassword(password_line.text)
        sync.authorizeDone()
        hideSubMessage()
    }

    function hide() {
        started = false
    }

    function back() {
        return true
    }

    Component.onCompleted: {
        started = true
        start_timer.restart()
    }

    Timer {
        id: start_timer
        interval: 600
        onTriggered: sync.start()
    }
}

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
import AsemanTools 1.0
import Papyrus 1.0

Item {
    id: security
    width: 100
    height: 62

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
    }

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 10*Devices.density
        anchors.verticalCenter: parent.verticalCenter
        spacing: 6*Devices.density

        Text{
            id: message
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.margins: 10*Devices.density
            font.pixelSize: 15*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
            wrapMode: Text.WordWrap
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        Rectangle {
            id: item
            width: parent.width
            height:  50*Devices.density
            color: marea.pressed? "#3B97EC" : "#00000000"

            Text{
                id: txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                y: parent.height/2 - height/2
                font.pixelSize: Devices.isMobile? 11*Devices.fontDensity : 13*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                color: marea.pressed? "#ffffff" : "#333333"
                wrapMode: TextInput.WordWrap
                text: qsTr("Numeric Password")
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: {
                    checkbox.checked = !checkbox.checked
                }
            }

            CheckBox {
                id: checkbox
                x: papyrus.languageDirection == Qt.RightToLeft? 20 : item.width - width - 20
                anchors.verticalCenter: parent.verticalCenter
                color: marea.pressed? "#ffffff" : "#333333"
                checked: database.passwordType() == 0
            }
        }

        Rectangle {
            id: pass_frame
            color: "white"
            smooth: true
            radius: 3*Devices.density
            height: 40*Devices.density
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20*Devices.density
            anchors.rightMargin: 20*Devices.density

            STextInput{
                id: pass_placeholder
                color: "#bbbbbb"
                font.pixelSize: pass.font.pixelSize
                font.family: AsemanApp.globalFontFamily
                y: pass.y
                anchors.left: pass.left
                anchors.right: pass.right
                anchors.margins: pass.anchors.margins
                visible: (!pass.focus && pass.text == "")
            }

            STextInput{
                id: pass
                color: "#333333"
                font.pixelSize: 11*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                y: pass_frame.height/2-height/2
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10*Devices.density
                echoMode: TextInput.Password
                inputMethodHints: checkbox.checked? Qt.ImhDigitsOnly : Qt.ImhNone
                onAccepted: pass_repeat.focus = true
            }
        }

        Rectangle {
            id: pass_repeat_frame
            color: "white"
            smooth: true
            radius: 3*Devices.density
            height: 40*Devices.density
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 20*Devices.density
            anchors.rightMargin: 20*Devices.density

            STextInput{
                id: pass_repeat_placeholder
                color: "#bbbbbb"
                font.pixelSize: pass_repeat.font.pixelSize
                font.family: AsemanApp.globalFontFamily
                y: pass_repeat.y
                anchors.left: pass_repeat.left
                anchors.right: pass_repeat.right
                anchors.margins: pass_repeat.anchors.margins
                visible: (!pass_repeat.focus && pass_repeat.text == "")
            }

            STextInput{
                id: pass_repeat
                color: "#333333"
                font.pixelSize: 11*Devices.fontDensity
                font.family: AsemanApp.globalFontFamily
                y: pass_repeat_frame.height/2-height/2
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10*Devices.density
                echoMode: TextInput.Password
                inputMethodHints: checkbox.checked? Qt.ImhDigitsOnly : Qt.ImhNone
                onAccepted: papyrus.hideKeyboard()
            }
        }

        Item{
            id: buttons_frame
            height: 40*Devices.density
            width: parent.width
            anchors.left: parent.left
            anchors.right: parent.right

            Button{
                id: done_btn
                height: 40*Devices.density
                width: buttons_frame.width/2 - 30*Devices.density
                anchors.left: parent.left
                anchors.top: buttons_frame.top
                anchors.bottom: buttons_frame.bottom
                anchors.leftMargin: 20*Devices.density
                fontSize: 9*Devices.fontDensity
                normalColor: "#66ffffff"
                textColor: "#333333"
                visible: pass.text == pass_repeat.text
                onClicked: {
                    database.setPassword( Tools.passToMd5(pass.text), checkbox.checked? Database.Numeric : Database.Characters )
                    showTooltip( qsTr("Password changed") )
                    papyrus_root.refresh()
                    popPreference()
                }
            }

            Button{
                id: cancel_btn
                height: 40*Devices.density
                width: done_btn.width
                anchors.left: done_btn.right
                anchors.top: buttons_frame.top
                anchors.bottom: buttons_frame.bottom
                anchors.leftMargin: 20*Devices.density
                fontSize: 9*Devices.fontDensity
                normalColor: "#66ffffff"
                textColor: "#333333"
                onClicked: popPreference()
            }
        }
    }

    Connections{
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        message.text = qsTr("You can set password from here for Papyrus start up.")
        pass_placeholder.text = qsTr("New password")
        pass_repeat_placeholder.text = qsTr("Repeat password")
        done_btn.text = qsTr("Ok")
        cancel_btn.text = qsTr("Cancel")
        title.text = qsTr("Security")
    }

    Component.onCompleted: {
        if( database.password().length != 0 )
            getPass(security).allowBack = true

        initTranslations()
    }
}

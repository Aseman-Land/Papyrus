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
import Papyrus 1.0
import AsemanTools 1.0

Item {
    width: 100
    height: 62

    property string defaultFile: ":/translations_sources/lang-en.ts"

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        onBeginBack: {
            tr_model.saveTo( papyrus.translatorFilePath() )
        }
    }

    Row {
        id: section_row
        anchors.top: parent.top
        anchors.right: parent.right
        height: done_section_text.height
        anchors.margins: 4*Devices.density

        Text {
            id: done_section_text
            font.pixelSize: 20*Devices.fontDensity
            font.family: AsemanApp.globalFont.family
            text: tr_model.doneCount + "/"
        }

        Text {
            id: count_section_text
            font.pixelSize: 12*Devices.fontDensity
            font.family: AsemanApp.globalFont.family
            anchors.bottom: parent.bottom
            text: tr_model.count
        }
    }

    TranslationModel {
        id: tr_model
        Component.onCompleted: reload()

        function reload() {
            file = ""
            if( papyrus.existsFile(papyrus.translatorFilePath()) )
                file = papyrus.translatorFilePath()
            else
                file = defaultFile
        }
    }

    ListView {
        id: tr_view
        anchors.top: title.bottom
        anchors.left: parent.left
        anchors.bottom: btns_row.top
        anchors.right: parent.right
        maximumFlickVelocity: View.flickVelocity
        clip: true
        spacing: 20*Devices.density
        model: tr_model

        delegate: Item {
            id: item
            width: tr_view.width
            height: column.height

            Column {
                id: column
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.verticalCenter: parent.verticalCenter
                anchors.margins: 20*Devices.density
                spacing: 2*Devices.density

                Text {
                    width: column.width
                    text: originalText
                    wrapMode: Text.WrapAtWordBoundaryOrAnywhere
                    font.pixelSize: 14*Devices.fontDensity
                    font.family: AsemanApp.globalFont.family
                    textFormat: Text.PlainText
                    color: "#333333"
                }

                Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: tr_text.height + 20*Devices.density<40*Devices.density? 40*Devices.density : tr_text.height + 20*Devices.density
                    color: "#88ffffff"

                    MouseArea {
                        anchors.fill: parent
                        onClicked: tr_text.focus = true
                    }

                    STextEdit {
                        id: tr_text
                        anchors.left: parent.left
                        anchors.right: parent.right
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.margins: 20*Devices.density
                        wrapMode: TextInput.WrapAnywhere
                        text: translation
                        font.pixelSize: 11*Devices.fontDensity
                        font.family: AsemanApp.globalFont.family
                        color: "#333333"
                        onTextChanged: translation = text
                    }
                }
            }
        }
    }

    ScrollBar {
        scrollArea: tr_view; height: tr_view.height; width: 6*Devices.density
        anchors.right: tr_view.right; anchors.top: tr_view.top; color: "#000000"
    }

    Row {
        id: btns_row
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        height: 42*Devices.density

        Column {
            id: clear_column
            width: parent.width/2
            height: acceptMode? btns_row.height*3 : btns_row.height
            anchors.bottom: parent.bottom
            clip: true

            property bool acceptMode: false

            Timer {
                id: accept_timer
                interval: 2000
                repeat: false
                onTriggered: clear_column.acceptMode = false
            }

            Behavior on height {
                NumberAnimation{ easing.type: Easing.OutCubic; duration: 300 }
            }

            Button {
                id: clear_btn
                height: btns_row.height
                width: clear_column.width
                color: "#ff5532"
                highlightColor: "#E34B2C"
                textColor: "#ffffff"
                text: qsTr("Clear")
                onClicked: {
                    clear_column.acceptMode = !clear_column.acceptMode
                    if( clear_column.acceptMode ) {
                        accept_timer.restart()
                        return
                    }

                    papyrus.removeFile(papyrus.translatorFilePath())
                    tr_model.reload()
                    showTooltip( qsTr("Cleared") )
                }
            }

            Button {
                id: clear_cancel_btn
                height: btns_row.height
                width: clear_column.width
                color: "#4098bf"
                highlightColor: "#3B8DB1"
                textColor: "#ffffff"
                text: qsTr("Cancel")
                onClicked: {
                    clear_column.acceptMode = false
                }
            }

            Rectangle {
                height: btns_row.height
                width: clear_column.width
                color: "#333333"

                Text {
                    anchors.centerIn: parent
                    font.pixelSize: 12*Devices.fontDensity
                    font.family: AsemanApp.globalFont.family
                    text: qsTr("Are you sure?")
                    color: "#ffffff"
                }
            }
        }

        Button {
            id: save_btn
            height: btns_row.height
            width: parent.width/2
            color: "#33ccad"
            highlightColor: "#50ab99"
            textColor: "#ffffff"
            text: qsTr("Save")
            onClicked: {
                tr_model.saveTo( papyrus.translatorFilePath() )
                showSubMessage(translation_save_component)
            }
        }
    }

    Component {
        id: translation_save_component
        TranslationSaveDialog {
            anchors.fill: parent
            anchors.topMargin: View.statusBarHeight
            anchors.bottomMargin: View.navigationBarHeight
            countries: tr_model.countries()
            languages: tr_model.languages()
            onLetsGo: {
                tr_model.language = currentLanguage
                tr_model.country = currentCountry
                tr_model.saveTo( papyrus.translatorFilePath() )

                var path = papyrus.getTempPath() + ".ts"

                papyrus.removeFile(path)
                papyrus.copyFile(papyrus.translatorFilePath(), path)

                showTooltip( qsTr("Saved Successfully on") + "\n" + papyrus.translatorFilePath() )
                hideSubMessage()
            }
        }
    }
}

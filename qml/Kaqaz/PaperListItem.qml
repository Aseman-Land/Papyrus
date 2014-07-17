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

Item {
    id: pitem
    width: 100
    height: 100

    property bool last: false
    property int idx: 0

    property int paperId: 0

    Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: -pback.shadowSize/3
        y: -pback.shadowSize
        height: pitem.last? fixedHeight : pitem.height + 2*pback.shadowSize
        clip: true

        PaperBackground {
            id: pback
            anchors.fill: parent
        }

        Rectangle {
            color: marea.pressed? "#220d80ec" : "#00000000"
            anchors.fill: parent
            anchors.margins: pback.shadowSize

            GroupChooserBack {
                id: gc_back
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.leftMargin: 15*physicalPlatformScale
                group: database.paperGroup(pitem.paperId)
            }

            Text {
                id: date_label
                anchors.top: parent.top
                anchors.right: parent.right
                anchors.margins: 7*physicalPlatformScale
                font.pointSize: 7*fontsScale
                font.family: globalFontFamily
                color: "#aaaaaa"
                text: kaqaz.convertDateTimeToString( database.paperCreatedDate(pitem.paperId) )
            }

            Text{
                id: title_txt
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.topMargin: 15*physicalPlatformScale
                anchors.leftMargin: gc_back.x + gc_back.width
                anchors.rightMargin: gc_back.x + gc_back.width
                font.pointSize: 14*fontsScale
                font.family: globalFontFamily
                color: title.length==0? "#cccccc" : "#333333"
                text: title.length==0? qsTr("Title") : title
                horizontalAlignment: Text.AlignHCenter

                property string title: database.paperTitle(pitem.paperId)
            }

            Text {
                id: body_txt
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom
                anchors.top: title_txt.bottom
                anchors.margins: 20*physicalPlatformScale
                anchors.topMargin: 0
                font.pointSize: 9*fontsScale
                font.family: globalFontFamily
                wrapMode: Text.Wrap
                smooth: true
                color: "#333333"
                text: database.paperText(pitem.paperId)
            }

            MouseArea {
                id: marea
                anchors.fill: parent
                onClicked: {
                    main.stackSwitcher.currentFrame.item.currentIndex = pitem.idx
                    main.popPreference()
                }
            }
        }
    }
}

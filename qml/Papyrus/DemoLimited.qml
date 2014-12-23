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

Item {
    id: dlimit
    anchors.fill: parent

    ListView {
        id: dlist
        anchors.fill: parent
        anchors.topMargin: View.statusBarHeight
        anchors.bottomMargin: View.navigationBarHeight
        orientation: Qt.Vertical
        snapMode: ListView.SnapOneItem
        rebound: Transition { NumberAnimation {duration: 0} }
        boundsBehavior: Flickable.StopAtBounds
        model: ListModel{}
        delegate: Item {
            id: item
            width: dlist.width
            height: dlist.height

            Component.onCompleted: {
                var component
                switch(idx) {
                case 0:
                    component = Qt.createComponent("DemoLimitedStartPage.qml")
                    component.createObject(item)
                    break;
                case 1:
                    component = Qt.createComponent("DemoLimitedBuyPro.qml")
                    component.createObject(item).buy.connect(dlimit.buy)
                    break;
                case 2:
                    component = Qt.createComponent("DemoLimitedActiveTrial.qml")
                    component.createObject(item).active.connect(dlimit.active)
                    break;
                }
            }
        }

        function refresh() {
            model.clear()
            for( var i=0; i<3; i++ )
                model.append({"idx":i})
        }

        Component.onCompleted: refresh()
    }

    ScrollBar {
        scrollArea: dlist; height: dlist.height; width: 6*Devices.density
        anchors.right: dlist.right; anchors.top: dlist.top; color: "#ffffff"
    }

    Column {
        id: troll_column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        visible: false

        Image {
            id: troll_img
            anchors.horizontalCenter: parent.horizontalCenter
            width: 256*Devices.density
            height: width
            sourceSize: Qt.size(width,height)
            fillMode: Image.PreserveAspectFit
        }

        Text {
            id: troll_text
            anchors.horizontalCenter: parent.horizontalCenter
            font.pixelSize: 20*Devices.fontDensity
            font.family: AsemanApp.globalFontFamily
            color: "#333333"
        }
    }

    Button {
        id: active_btn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom
        anchors.leftMargin: 40*Devices.density
        anchors.rightMargin: 40*Devices.density
        anchors.bottomMargin: 20*Devices.density + View.navigationBarHeight
        height: 42*Devices.density
        normalColor: "#4098bf"
        highlightColor: "#337fa2"
        text: qsTr("Dismiss")
        visible: false
        onClicked: active()
    }

    Indicator {
        id: indicator
        anchors.centerIn: parent
        width: 256*Devices.density
        height: width
        light: false
    }

    Timer {
        id: idle_timer
        interval: 2000
        repeat: false
        onTriggered: {
            troll_column.visible = true
            active_btn.visible = troll_img.visible
            indicator.stop()
        }
    }

    Timer {
        id: close_timer
        interval: 5000
        repeat: false
        onTriggered: hideSubMessage()
    }

    function active() {
        troll_column.visible = false
        active_btn.visible = false
        dlist.visible = false
        troll_img.visible = false
        troll_text.text = qsTr("ACTIVED")
        indicator.start()
        idle_timer.restart()
        close_timer.restart()
        papyrus.demoActiveTrial()
    }

    function buy() {
        troll_column.visible = false
        active_btn.visible = false
        dlist.visible = false
        troll_img.source = "files/buy-troll.png"
        troll_text.text = qsTr("THANK YOU")
        indicator.start()
        idle_timer.restart()
        Qt.openUrlExternally("market://details?id=land.aseman.papyrus")
    }

    function back() {
        return false
    }

    Component.onCompleted: BackHandler.pushHandler(dlimit,dlimit.back)
}

import QtQuick 2.2
import QtQuick.Controls 1.1

ApplicationWindow {
    id: main
    visible: true
    width: 1280
    height: 720
    flags: Qt.CustomizeWindowHint

    property int frame: 0
    property real papersWidth: 420
    property real papersHeight: 620

    Timer {
        interval: 100
        repeat: true
        onTriggered: main.frame++
        Component.onCompleted: start()
    }

    Image {
        anchors.centerIn: parent
        source: "files/background.jpg"
        fillMode: Image.PreserveAspectCrop
        width: sourceSize.width
        height: sourceSize.height
    }

    Logo {
        x: main.frame<60 || main.frame>352? main.width/2-width/2 : 30
        y: main.frame<373 || main.frame>403? main.height/2-height/2 : 60
        opacity: main.frame<525? 1 : 0

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 1000 }
        }
        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    Text {
        anchors.centerIn: parent
        font.family: "Ubuntu"
        font.pointSize: 40
        color: "#333333"
        text: "Enjoy it"
        opacity: main.frame<540 || main.frame>560? 0 : 1

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.Linear; duration: 600 }
        }
    }

    Text {
        anchors.top: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 80
        font.family: "Ubuntu"
        font.pointSize: 24
        color: "#333333"
        text: "Searching \"Papyrus\"..."
        opacity: main.frame<353 || main.frame>373? 0 : 1
        scale: main.frame<353 || main.frame>373? 1.05 : 1

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    Text {
        anchors.top: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 80
        font.family: "Ubuntu"
        font.pointSize: 18
        color: "#333333"
        text: "You can also discover notes using labels, date and location soon."
        opacity: main.frame<413 || main.frame>443? 0 : 1
        scale: main.frame<413 || main.frame>443? 1.05 : 1

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    Text {
        anchors.top: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 80
        font.family: "Ubuntu"
        font.pointSize: 18
        color: "#333333"
        text: "Papyrus is a multi platform application and running on Windows, Linux, Mac, Android and iOS soon."
        opacity: main.frame<453 || main.frame>473? 0 : 1
        scale: main.frame<453 || main.frame>473? 1.05 : 1

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    Text {
        anchors.top: parent.verticalCenter
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 80
        font.family: "Ubuntu"
        font.pointSize: 18
        color: "#333333"
        text: "Papyrus also has encrypted synchronization feature via Dropbox among all your devices"
        opacity: main.frame<483 || main.frame>503? 0 : 1
        scale: main.frame<483 || main.frame>503? 1.05 : 1

        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on scale {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    PaperBackground {
        id: paper1
        x: main.frame<59 || main.frame>352? parent.width + 50 : 200
        y: main.frame<353? parent.height/2-height/2 : parent.height + 50
        rotation: main.frame<59? -10 : (main.frame>352? 5 : 0)
        anchors.verticalCenter: parent.verticalCenter
        width: papersWidth
        height: papersHeight

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on rotation {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    PaperBackground {
        id: paper2
        x: main.frame<60 || main.frame>350? parent.width + 50 : 200
        y: main.frame<353? parent.height/2-height/2 : parent.height + 50
        anchors.verticalCenter: parent.verticalCenter
        width: papersWidth
        height: papersHeight
        rotation: main.frame<60 || main.frame>350? 10 : (main.frame>270? 0 : -2)
        text: main.frame<280? "" : "Ask"
        bodyText: main.frame<290? "" : "Just ask Papyrus what you want.          \n"+
                                  "you can search it, browse using labels, history and ..."

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on rotation {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    PaperBackground {
        id: paper3
        x: main.frame<61 || main.frame>270? (main.frame<360? parent.width + 50 : parent.width/2 + 10 ) : 200
        y: main.frame<353? parent.height/2-height/2 : (main.frame<372 || main.frame>400? parent.height + 50 : 250)
        width: papersWidth
        height: papersHeight
        rotation: main.frame<61 || main.frame>270? (main.frame<360? 10 : 0) : (main.frame>175? 0 : 2)
        text: main.frame<185? "" : "Just Write"
        bodyText: main.frame<195? "" : "Only think about writing. Write on and on and fill in thousands of papers with a distress-free mind."
        image: main.frame<260? "" : "files/audio.png"

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on rotation {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    PaperBackground {
        id: paper4
        x: main.frame<62 || main.frame>175? (main.frame<360? parent.width + 50 : parent.width/2 - width - 10 ) : 200
        y: main.frame<353? parent.height/2-height/2 : (main.frame<372 || main.frame>400? parent.height + 50 : 250)
        width: papersWidth
        height: papersHeight
        rotation: main.frame<62? -10 : (main.frame>175 && main.frame<360? 10 : 0)
        text: main.frame<70? "" : "Papyrus intro"
        bodyText: main.frame<80? "" : "Write your notes, memories, to-do, ... and go to the next paper using cool gestures." +
                                 "\n                 \nYou can also attach images or audios to papers."
        image: main.frame<165? "" : "files/img.png"

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on rotation {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    Image {
        anchors.centerIn: parent
        source: "files/background.jpg"
        fillMode: Image.PreserveAspectCrop
        width: sourceSize.width
        height: sourceSize.height
        opacity: main.frame<30? 1 : 0
        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
    }

    Image {
        x: large? main.width/2-largeWidth/2 : main.width - smallWidth - 10
        y: large? main.height/2-largeHeight/2 : main.height - smallHeight - 10
        width: large? largeWidth :smallWidth
        height: large? largeHeight :smallHeight
        source: "files/aseman-labs.png"
        sourceSize: Qt.size(large?largeWidth:smallWidth ,large?largeHeight:smallHeight )
        opacity: main.frame<540 || main.frame>570? 1 : 0

        property real smallWidth: 181
        property real smallHeight: 26

        property real largeWidth: 425
        property real largeHeight: 61

        property bool large: main.frame<30 || main.frame>550

        Behavior on x {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on y {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on width {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on height {
            NumberAnimation{ easing.type: Easing.OutCubic; duration: 600 }
        }
        Behavior on opacity {
            NumberAnimation{ easing.type: Easing.Linear; duration: 600 }
        }
    }
}

import QtQuick 2.0
import AsemanTools 1.0

Column {
    id: column
    anchors.verticalCenter: parent.verticalCenter
    anchors.left: parent.left
    anchors.right: parent.right

    Text {
        width: main.width - 40*Devices.density
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.margins: 10*Devices.density
        font.family: AsemanApp.globalFont.family
        font.pixelSize: 9*Devices.fontDensity
        color: "#333333"
        wrapMode: Text.WrapAtWordBoundaryOrAnywhere
        text: qsTr("Thank you for choosing Papyrus.\nIf you are like this app, please rate us on Google play or Bazaar.\nThank you.")
    }

    Row {
        anchors.right: parent.right
        Button {
            textFont.family: AsemanApp.globalFont.family
            textFont.pixelSize: 10*Devices.fontDensity
            textColor: "#0d80ec"
            normalColor: "#00000000"
            highlightColor: "#660d80ec"
            text: qsTr("Cancel")
            onClicked: {
                BackHandler.back()
            }
        }

        Button {
            textFont.family: AsemanApp.globalFont.family
            textFont.pixelSize: 10*Devices.fontDensity
            textColor: "#0d80ec"
            normalColor: "#00000000"
            highlightColor: "#660d80ec"
            text: qsTr("OK")
            onClicked: {
                if(papyrus.proBuild)
                    Qt.openUrlExternally("market://details?id=land.aseman.papyrus")
                else
                    Qt.openUrlExternally("market://details?id=land.aseman.papyrus.free")
                BackHandler.back()
            }
        }
    }
}

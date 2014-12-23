import QtQuick 2.0

Item {
    id: spw_item
    width: 100
    height: 50

    property string weather

    GridView {
        id: pw_list
        anchors.fill: parent
        flow: GridView.FlowTopToBottom
        layoutDirection: Qt.LeftToRight
        model: ListModel{}
        cellWidth: cellHeight
        cellHeight: height/2

        delegate: Item {
            width: pw_list.cellWidth
            height: pw_list.cellHeight

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
                    spw_item.weather = iconId
                }
            }
        }

        Component.onCompleted: {
            pw_list.model.append( {"iconId": "weather-clear"} )
            pw_list.model.append( {"iconId": "weather-clouds"} )
            pw_list.model.append( {"iconId": "weather-overcast"} )
            pw_list.model.append( {"iconId": "weather-showers"} )
            pw_list.model.append( {"iconId": "weather-storm"} )
            pw_list.model.append( {"iconId": "weather-snow"} )
            pw_list.model.append( {"iconId": "weather-fog"} )
            pw_list.model.append( {"iconId": "weather-unknown"} )
        }
    }
}

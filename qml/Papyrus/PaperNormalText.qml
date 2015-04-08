import QtQuick 2.0
import Papyrus 1.0
import AsemanTools 1.0

Item {
    width: 100
    height: 62

    property alias text: tdmodel.text
    property alias alignment: txt.horizontalAlignment
    property alias font: txt.font
    property alias color: txt.color
    property alias interactive: flick.interactive

    Flickable {
        id: flick
        anchors.fill: parent
        contentWidth: txt.paintedWidth
        contentHeight: txt.paintedHeight
        flickableDirection: Flickable.VerticalFlick
        clip: true

        TextAreaCore {
            id: txt
            width: flick.width
            height: flick.height
            focus: true
            wrapMode: TextEdit.Wrap
            onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
        }

        function ensureVisible(r)
        {
            if (contentX >= r.x)
                contentX = r.x;
            else if (contentX+width <= r.x+r.width)
                contentX = r.x+r.width-width;
            if (contentY >= r.y)
                contentY = r.y;
            else if (contentY+height <= r.y+r.height)
                contentY = r.y+r.height-height;
        }
    }
}


import QtQuick 2.0
import Papyrus 1.0
import AsemanTools 1.0

PapyrusPaper {
    id: paper
    width: 100
    height: 62
    anchors.fill: parent

    property string text: paperCore.text

    onTextChanged: txt.text = text

    Flickable {
        id: flick
        width: parent.width
        height: Devices.keyboard? parent.height-Devices.keyboardHeight : parent.height
        contentWidth: txt.paintedWidth
        contentHeight: txt.paintedHeight
        flickableDirection: Flickable.VerticalFlick
        interactive: paper.interactive
        onHeightChanged: ensureVisible(txt.cursorRectangle)
        onContentYChanged: if(txt.selectionStart == txt.selectionEnd) txt.hidePicker()

        TextAreaCore {
            id: txt
            width: flick.width
            height: flick.contentHeight<flick.height? flick.height : flick.contentHeight
            textMargin: 10*Devices.density
            focus: true
            wrapMode: TextEdit.Wrap
            onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
            onTextChanged: if(init_timer.inited) paperCore.text = text
            onPressChanged: paper.interactive = !press
            font.family: papyrus.bodyFont.family

            Timer {
                id: init_timer
                interval: 500
                onTriggered: inited = true
                Component.onCompleted: start()

                property bool inited: false
            }
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


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

import QtQuick 2.2
import SialanTools 1.0

Item {
    id: item
    width: img.width
    height: img.height
    anchors.centerIn: parent

    property string fileId: fid
    property string path: filePath

    property alias imgItem: img

    signal deleteRequest( variant item )

    Image{
        id: img
        anchors.centerIn: parent
        asynchronous: true
        width: smaller? sourceSize.width : (ratio>1? frameWidth : frameHeight*ratio)
        height: smaller? sourceSize.height : (ratio>1? frameWidth/ratio : frameHeight)
        fillMode: Image.PreserveAspectFit
        source: "file://" + path

        property real frameWidth: frameHeight
        property real frameHeight: item.parent.height
        property real ratio: sourceSize.width/sourceSize.height
        property bool smaller: (sourceSize.width<frameWidth && sourceSize.height<frameHeight)
    }
}

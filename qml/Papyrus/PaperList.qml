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
    id: paper_list
    anchors.fill: parent

    property variant list: new Array

    onListChanged: p_list.refresh()

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        z: 10
    }

    ListView {
        id: p_list
        anchors.fill: parent
        anchors.topMargin: -View.statusBarHeight
        anchors.bottomMargin: -View.navigationBarHeight
        model: ListModel{}
        maximumFlickVelocity: View.flickVelocity
        clip: true

        property real itemHeight: 100*Devices.density

        header: Item{
            width: p_list.width
            height: (Devices.isAndroid? 20 + View.statusBarHeight : title.height)
        }

        footer: Item{
            width: p_list.width
            height: View.navigationBarHeight
        }

        delegate: PaperListItem {
            id: item
            width: p_list.width
            height: p_list.itemHeight
            last: index == p_list.model.count-1
            z: index
            idx: index
            paperId: pid
        }

        function refresh() {
            for( var i=0; i<paper_list.list.length; i++ )
                model.append({"pid":paper_list.list[i]})
        }

        Component.onCompleted: refresh()
    }

    ScrollBar {
        scrollArea: p_list; height: p_list.height; width: 6*Devices.density
        anchors.right: p_list.right; anchors.top: p_list.top; color: "#000000"
    }

    function positionViewAtIndex( idx ) {
        var itemsPerPage = Math.floor(p_list.height/p_list.itemHeight)
        var pad = (itemsPerPage/2>2? 2 : 1)

        if( idx<pad )
            p_list.positionViewAtBeginning()
        else
            p_list.positionViewAtIndex( idx-pad, ListView.Beginning )
    }
}

/*
    Copyright (C) 2014 Sialan Labs
    http://labs.sialan.org

    This project is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This project is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

import QtQuick 2.0
import SialanTools 1.0

Item {
    id: time_domain
    width: 100
    height: 62

    property alias startTime: start_tm
    property alias endTime: end_tm

    Row {
        id: row
        anchors.fill: parent
        anchors.margins: 20*physicalPlatformScale
        spacing: 20*physicalPlatformScale

        DateTimeChooser {
            id: start_tm
            timeVisible: true
            dateVisible: false
            width: parent.width/2 - row.spacing/2
            height: parent.height
            color: "#D9D9D9"
            textsColor: "#111111"
            timeLabel: qsTr("Start Time")
        }

        DateTimeChooser {
            id: end_tm
            timeVisible: true
            dateVisible: false
            width: parent.width/2 - row.spacing/2
            height: parent.height
            color: "#D9D9D9"
            textsColor: "#111111"
            timeLabel: qsTr("End Time")
        }
    }
}

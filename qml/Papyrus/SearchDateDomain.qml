/*
    Copyright (C) 2014 Aseman
    http://aseman.co

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
import AsemanTools 1.0

Item {
    id: date_domain
    width: 100
    height: 62

    property alias startDate: start_dt
    property alias endDate: end_dt

    Row {
        id: row
        anchors.fill: parent
        anchors.margins: 20*Devices.density
        spacing: 20*Devices.density

        DateTimeChooser {
            id: start_dt
            timeVisible: false
            dateVisible: true
            width: parent.width/2 - row.spacing/2
            height: parent.height
            color: "#D9D9D9"
            textsColor: "#111111"
            dateLabel: qsTr("Start Date")
        }

        DateTimeChooser {
            id: end_dt
            timeVisible: false
            dateVisible: true
            width: parent.width/2 - row.spacing/2
            height: parent.height
            color: "#D9D9D9"
            textsColor: "#111111"
            dateLabel: qsTr("End Date")
        }
    }
}

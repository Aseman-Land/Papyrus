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
    id: move_wait

    property string path

    Text {
        id: status_text
        y: move_wait.height/2 - height/2
        anchors.left: move_wait.left
        anchors.right: move_wait.right
        anchors.margins: 20*physicalPlatformScale
        font.pixelSize: 15*fontsScale
        font.family: SApp.globalFontFamily
        horizontalAlignment: Text.AlignHCenter
        color: "#333333"
    }

    Timer{
        id: move_start
        interval: 1000
        repeat: false
        onTriggered: {
            kaqaz.setProfilePath( move_wait.path )
            success()
        }
    }

    Timer{
        id: close_timer
        interval: 1000
        repeat: false
        onTriggered: {
            main.popPreference()
        }
    }

    function back(){
        return true
    }

    function success(){
        status_text.text  = qsTr("Done Successfully")
        close_timer.start()
    }

    function failed(){
        status_text.text  = qsTr("Failed!")
        close_timer.start()
    }

    Connections{
        target: kaqaz
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        status_text.text  = qsTr("Please Wait")
    }

    Component.onCompleted: {
        initTranslations()
        move_start.start()
        backHandler = move_wait
    }
}

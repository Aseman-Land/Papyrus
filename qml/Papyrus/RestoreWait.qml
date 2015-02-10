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

import QtQuick 2.2
import AsemanTools 1.0
import Papyrus 1.0

Item {
    id: restore_wait

    property string path
    property variant progress

    Backuper {
        id: backuper
        onSuccess: restore_wait.success()
        onFailed: restore_wait.failed()
        onProgress: restore_wait.progress.setValue(percent)
    }

    Text {
        id: status_text
        anchors.verticalCenter: restore_wait.verticalCenter
        anchors.left: restore_wait.left
        anchors.right: restore_wait.right
        anchors.margins: 20*Devices.density
        font.pixelSize: 15*Devices.fontDensity
        font.family: AsemanApp.globalFont.family
        horizontalAlignment: Text.AlignHCenter
        color: "#333333"
    }

    Timer{
        id: restore_start
        interval: 1000
        repeat: false
        onTriggered: {
            var ok = backuper.restore( restore_wait.path, "" )
            if( !ok ) {
                var passItem = getPass(restore_wait)
                passItem.getPassOnly = true
                passItem.inputMethodHints = Qt.ImhNone
                passItem.allowBack = true
                passItem.passGiven.connect(restore_wait.passGiven)
            }
            else
                BackHandler.pushHandler(restore_wait,restore_wait.back)
        }
    }

    Timer {
        id: close_timer
        interval: 1000
        repeat: false
        onTriggered: {
            main.popPreference()
            progress.destroy()
        }
    }

    Timer {
        id: restore_timer
        interval: 250
        repeat: false
        onTriggered: {
            var ok = backuper.restore( restore_wait.path, Tools.passToMd5(password) )
            if( !ok )
                restore_wait.failed()
            else
                BackHandler.pushHandler(restore_wait,restore_wait.back)
        }

        property string password
    }

    function back(){
        return false
    }

    function passGiven( pass ){
        restore_timer.password = pass
        restore_timer.restart()
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
        target: papyrus
        onLanguageChanged: initTranslations()
    }

    function initTranslations(){
        status_text.text  = qsTr("Please Wait")
    }

    Component.onCompleted: {
        initTranslations()
        restore_start.start()
        progress = newModernProgressBar()
    }

    Component.onDestruction: if(progress) progress.destroy()
}


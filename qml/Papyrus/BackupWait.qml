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
    id: backup_wait

    property variant progress

    Backuper {
        id: backuper
        onSuccess: backup_wait.success()
        onFailed: backup_wait.failed()
        onProgress: backup_wait.progress.setValue(percent)
    }

    Text {
        id: status_text
        anchors.verticalCenter: backup_wait.verticalCenter
        anchors.left: backup_wait.left
        anchors.right: backup_wait.right
        anchors.margins: 20*Devices.density
        font.pixelSize: 15*Devices.fontDensity
        font.family: AsemanApp.globalFont.family
        horizontalAlignment: Text.AlignHCenter
        color: "#333333"
    }

    Timer{
        id: backup_start
        interval: 1000
        repeat: false
        onTriggered: {
            backuper.makeBackup( database.password() )
            BackHandler.pushHandler(backup_wait,backup_wait.back)
        }
    }

    Timer{
        id: close_timer
        interval: 1000
        repeat: false
        onTriggered: {
            main.popPreference()
            progress.destroy()
        }
    }

    function back(){
        return false
    }

    function success(){
        if( sync.tokenAvailable )
            sync.refresh()
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
        progress = newModernProgressBar()
        backup_start.start()
    }

    Component.onDestruction: if(progress) progress.destroy()
}

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
import QtMultimedia 5.0
import AsemanTools 1.0

Rectangle {
    id: camera_item
    width: 100
    height: 62
    color: "#000000"
    anchors.fill: parent

    Header {
        id: title
        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        light: true
    }

    Camera {
        id: camera

        imageProcessing.whiteBalanceMode: CameraImageProcessing.WhiteBalanceFlash

        exposure {
            exposureCompensation: -1.0
            exposureMode: Camera.ExposurePortrait
        }

        flash.mode: Camera.FlashRedEyeReduction

        imageCapture {
            onImageSaved: {
                camera_item.destroy()
            }
        }
    }

    VideoOutput {
        source: camera
        anchors.fill: parent
//        rotation: 90
        focus : visible
    }

    Button {
        id: captureBtn
        height: 100*Devices.density
        width: height
        radius: height/2
        normalColor: "#33ffffff"
        icon: "files/camera.png"
        iconHeight: 60*Devices.density
        anchors.horizontalCenter: camera_item.horizontalCenter
        anchors.bottom: camera_item.bottom
        anchors.bottomMargin: 20*Devices.density
        onClicked: camera.imageCapture.captureToLocation( Devices.cameraLocation + "/test.jpg" )
    }
}

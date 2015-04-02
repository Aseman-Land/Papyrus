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
import Papyrus 1.0
import AsemanTools 1.0

Rectangle {
    id: sidebar_list
    width: 100
    height: 62
    color: "#00000000"

    signal itemSelected( int id, int type )
    signal commandSelected()

    property color fontColor: "#333333"

    property variant cacheDate

    ListView {
        id: list_view
        anchors.fill: parent
        anchors.topMargin: 20*Devices.density
        anchors.bottomMargin: 20*Devices.density
        highlightMoveDuration: 250
        clip: true
        maximumFlickVelocity: View.flickVelocity

        model: SideBarModel {
            id: sidebar_model
            Component.onCompleted: refreshActions()

            function refreshActions() {
                clearActions()
                addAction(SideBarModel.SectionAction, qsTr("Clean Papers"), "files/clean-papers.png", main.clean)
                if( papyrus.allPaper )
                    addAction(SideBarModel.SectionAction, qsTr("All Papers"), "files/all-papers.png", showAll)

                addAction(SideBarModel.SectionAction, qsTr("Search"), "files/search.png", main.showSearch)
                if( Devices.isDesktop ) {
                    if( database.password().length != 0 )
                        addAction(SideBarModel.SectionAction, qsTr("Lock"), "files/lock.png", lock)

                    addAction(SideBarModel.SectionAction, qsTr("Fullscreen"), "files/fullscreen.png", fullscreen)
                }
                if( sync.tokenAvailable )
                    addAction(SideBarModel.SectionAction, qsTr("Force Sync"), "files/sync.png", forceSync)

                addAction(SideBarModel.SectionAction, qsTr("Preferences"), "files/preferences.png", main.showPrefrences)
                addAction(SideBarModel.SectionHistory, qsTr("History"), "files/history.png", main.showHistory)
            }
        }

        section.property: "section"
        section.criteria: ViewSection.FullString
        section.delegate: Item {
            width: list_view.width
            height: section != SideBarModel.SectionAction? 32*Devices.density : 0
        }

        delegate: Rectangle {
            id: item
            width: list_view.width
            height: 32*Devices.density
            color: marea.pressed?"#3B97EC" : "#00000000"
            radius: 3*Devices.density

            Rectangle {
                id: color_rect
                anchors.top: item.top
                anchors.bottom: item.bottom
                anchors.margins: 1*Devices.density
                anchors.left: icon_img.left
                width: 7*Devices.density
                color: model.color
                visible: model.type == SideBarModel.TypeGroup
            }

            Image {
                id: icon_img
                height: 22*Devices.density
                width: height
                sourceSize: Qt.size(width,height)
                anchors.left: item.left
                anchors.leftMargin: 6*Devices.density
                anchors.verticalCenter: item.verticalCenter
                source: model.type == SideBarModel.TypeHistory? "files/history.png" : model.icon
                smooth: true
                visible: model.type != SideBarModel.TypeGroup
            }

            Text{
                id: txt
                anchors.left: (icon_img.source != "")? icon_img.right : color_rect.right
                anchors.right: parent.right
                anchors.margins: 30*Devices.density
                anchors.leftMargin: 6*Devices.density
                y: parent.height/2 - height/2
                text: model.type != SideBarModel.TypeAction? model.name + " (" + model.papersCount + ")" : model.name
                font.pixelSize: 9*Devices.fontDensity
                font.family: AsemanApp.globalFont.family
                horizontalAlignment: Qt.AlignLeft
                color: marea.pressed? "#ffffff" : fontColor
            }

            MouseArea{
                id: marea
                anchors.fill: parent
                onClicked: {
                    switch(model.type)
                    {
                    case SideBarModel.TypeAction:
                        model.command()
                        sidebar_list.commandSelected( model.command )
                        break;

                    case SideBarModel.TypeGroup:
                        sidebar_list.itemSelected(model.groupId, PaperManager.Group)
                        break;

                    case SideBarModel.TypeHistory:
                        sidebar_list.itemSelected(model.date, PaperManager.Date)
                        break;
                    }
                }
            }
        }

        focus: true
        currentIndex: -1
    }

    ScrollBar {
        scrollArea: list_view; height: list_view.height; width: 6*Devices.density
        anchors.right: list_view.right; anchors.top: list_view.top; color: "#ffffff"
    }

    Connections{
        target: papyrus
        onLanguageChanged: sidebar_list.refresh()
        onGroupsCountChanged: sidebar_list.refresh()
        onAllPaperChanged: sidebar_list.refresh()
    }

    Timer {
        id: show_list_timer
        interval: portrait? 1000 : 250
        onTriggered: main.showListView()
    }

    function refresh(){
        sidebar_model.refreshActions()
    }

    function showAll() {
        main.setCurrentGroup(0, PaperManager.All)
        show_list_timer.restart()
    }

    function fullscreen() {
        papyrus.fullscreen = !papyrus.fullscreen
    }

    function forceSync() {
        if( sync.tokenAvailable ) {
            sync.refreshForce()
            syncProgressBar.visible = true
        }
    }
}

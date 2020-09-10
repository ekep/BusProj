/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0
import QtQuick.Window 2.0
import QtLocation 5.12
import QtPositioning 5.12
import QtQuick.Shapes 1.1
import QtQuick.Controls 2.15


Window {
    id: window
    readonly property bool inPortrait: window.width < window.height

    width: 512
    height: 512
    property alias line1b: line1b
    visible: true

    function openDrawer() {
        drawer.open();
    }

    function closeDrawer() {
        drawer.close();
    }

    Plugin {
        id: mapPlugin
        name: "osm"
    }

    Drawer {
        id: drawer
        objectName: "drawer"
        width: 0.66 * window.width
        height: window.height
        dragMargin: 4
        Column {
            id: col
            x: 10
            objectName: "col";
            property alias l1a: line1a.text;
            Text {
                id: line1a
                x: 20
                y: 19
                objectName: "line1a"
                font.pixelSize: 24
            }
            TextArea {
                id: line1b
                x: 30
                objectName: "line1b"
                font.pixelSize: 16
            }
        }
    }

    Map {
        id: map
        zoomLevel: 13.5
        anchors.fill: parent
        visible: true
        //activeMapType: map.supportedMapTypes[2]
        plugin: mapPlugin
        center: QtPositioning.coordinate(65.065811,25.42562)
        MouseArea {
            anchors.fill: parent

            Connections {
                onClicked: backEnd.postMessage(map.toCoordinate(Qt.point(mouse.x, mouse.y)))
            }
        }

        MapItemView {
            id: mapview
            objectName: "mapview"
            anchors.fill: parent
            property alias map_path: poly.path
            MapCircle {
                color: 'red'
                border.color: 'red'
                radius: 3
                objectName: "curloc"
                center {
                    latitude: 65.065811;
                    longitude: 25.42562;
                }
            }

            MapPolyline {
                id: poly
                line.color: 'red'
                line.width: 3
            }
        }
    }
    function loadPath(){
        var lines = []
        for(var i=0; i < backEnd.geopath.size(); i++){
            lines[i] = backEnd.geopath.coordinateAt(i)
        }
        return lines;
    }
    Connections{
        target: backEnd
        onGeopathChanged: {
            console.log("loading...")
            poly.path = loadPath()
          }
    }
    Component.onCompleted: {
        //poly.path = loadPath()
      }
}


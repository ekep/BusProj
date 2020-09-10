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


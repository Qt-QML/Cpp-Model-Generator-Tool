import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtGraphicalEffects 1.0
import QtQml.Models 2.1

import CustomCPP 1.0

Item {
    anchors.fill: parent

    ScrollView {
        id: canvas
        anchors.bottom: parent.bottom
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.topMargin: 30
        clip: true

        Item {
            id: tmp
            width: childrenRect.width
            height: childrenRect.height

            transform: [ trans ]
            Scale {
                id: trans
            }

            Repeater {
                model: modelRoot.classes;
                Rectangle {
                    property QtObject rect: modelRoot.classes.get(index);
                    property point position: rect.pos
                    property bool singleRoot: rect.name === modelRoot.name

                    onRectChanged: console.log(rect.name)

                    Behavior on x { NumberAnimation { duration: 200; easing.type: Easing.OutQuad; }}
                    Behavior on y { NumberAnimation { duration: 200; easing.type: Easing.OutQuad; }}

                    color: "white"


                    onPositionChanged: { if(!mouse.drag.active) {x=position.x; y=position.y; }}

                    onXChanged: if (mouse.drag.active) rect.pos = Qt.point(x,y);
                    onYChanged: if (mouse.drag.active) rect.pos = Qt.point(x,y);

                    width: 250
                    height: view.height + view.anchors.topMargin



                    MouseArea {
                        id: mouse
                        anchors.fill: parent
                        drag.target: parent
                    }
                    Rectangle {
                        MyDropItem
                        {
                            anchors.fill: parent;
                            anchors.margins: -6

                            obj: rect
                        }

                        color: "#20000000"

                        anchors.top: parent.top
                        anchors.topMargin:2

                        visible:  !singleRoot

                        anchors.left: parent.left
                        anchors.leftMargin: -16
                        width: 20
                        height: 15

                        Text {
                            anchors.centerIn: parent
                            color: "black"
                            opacity: 1.0
                            //text: (row.type === "QObject*" ? "1" : "*")
                        }
                    }

                    Text {
                        text: rect.name
                        color: (singleRoot? "#a00000": "black");
                    }


                    Component {
                        id: dragDelegate

                        MouseArea {
                            id: dragArea

                            property bool held: false

                            anchors { left: parent.left; right: parent.right }
                            height: content.height

                            drag.target: held ? content : undefined
                            drag.axis: Drag.YAxis

                            onPressed:  held = true
                            onReleased: held = false

                            Rectangle {
                                id: content
                                anchors {
                                    horizontalCenter: parent.horizontalCenter
                                    verticalCenter: parent.verticalCenter
                                }

                                width: dragArea.width;
                                height: 18

                                color: (dragArea.held ? "lightsteelblue": (row.save ==false ? "#10A00000" : (index %2 == 0 ? "#10000000":"#08000000")));
                                Behavior on color { ColorAnimation { duration: 100 } }

                                radius: 2
                                Drag.active: dragArea.held
                                Drag.source: dragArea
                                Drag.hotSpot.x: width / 2
                                Drag.hotSpot.y: height / 2
                                states: State {
                                    when: dragArea.held

                                    ParentChange { target: content; parent: dragArea.parent }
                                    AnchorChanges {
                                        target: content
                                        anchors { horizontalCenter: undefined; verticalCenter: undefined }
                                    }
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.left: parent.left
                                    anchors.leftMargin: 8
                                    font.pixelSize: 10
                                    font.family: "Courier   "
                                    text: (row.read ? "R ": "  ") +
                                          (row.write ? "W ": "  ") +
                                          (row.notify ? "N ": "  ") +
                                          (row.undo ? "U ": "  ") +
                                          (row.save ? "S ": "  ") ;
                                }

                                Text {
                                    anchors.verticalCenter: parent.verticalCenter
                                    anchors.right: parent.right
                                    anchors.rightMargin: 8

                                    font.pointSize: 12
                                    text: (row.subType.length === 0? row.type : "") + (row.subType!==undefined? row.subType : "") +  "  " + row.name
                                }

                            }


                            // for reordering
                            DropArea {
                                anchors { fill: parent; margins: 2 }

                                onEntered: {
                                    view.model.moveWithUndo(
                                                drag.source.DelegateModel.itemsIndex,
                                                dragArea.DelegateModel.itemsIndex)
                                }
                            }
                            // dropsite for pointer types
                            Rectangle {
                                visible: row.type === "QObject*" || row.type === "ObjectList*"

                                MyDropItem
                                {
                                    anchors.fill: parent;
                                    anchors.margins: -6

                                    obj: row
                                }

                                color: "#20000000"

                                anchors.top: parent.top
                                anchors.topMargin:2
                                anchors.bottom: parent.bottom
                                anchors.bottomMargin:2
                                anchors.right: parent.right
                                anchors.rightMargin: -16
                                width: Math.max(20, txt.paintedWidth);

                                Text {
                                    id: txt
                                    anchors.centerIn: parent
                                    color: "black"
                                    opacity: 1.0
                                    text: (row.type === "QObject*" ? "1" : (row.count > 0 ? "*("+row.count+")" :"*"))
                                }
                            }
                        }
                    }


                    //                DelegateModel {
                    //                    id: visualModel

                    //                    model: rect.properties
                    //                    delegate: dragDelegate
                    //                }
                    ListView {
                        id: view

                        interactive: false
                        height: contentHeight
                        anchors {
                            top: parent.top;
                            left: parent.left;
                            right: parent.right;
                            topMargin: 22;
                        }

                        model: rect.properties
                        delegate: dragDelegate

                        displaced: Transition {
                            NumberAnimation { properties: "y"; duration: 400; easing.type: Easing.OutBounce }

                            // ensure opacity and scale values return to 1.0
                            NumberAnimation { property: "opacity"; to: 1.0 }
                            NumberAnimation { property: "scale"; to: 1.0 }
                        }
                        add: Transition {
                            NumberAnimation { property: "opacity"; from: 0; to: 1.0; duration: 200 }
                            NumberAnimation { property: "scale"; from: 0.7; to: 1.0; duration: 200 }
                        }
                        move: Transition {
                            NumberAnimation { properties: "y"; duration: 200; easing.type: Easing.OutBack }
                        }
                        remove:  Transition {
                            NumberAnimation { property: "opacity"; to: 0; from: 1.0; duration: 200 }
                            NumberAnimation { property: "scale"; to: 0.7; from: 1.0; duration: 200 }
                        }


                        spacing: 4
                        cacheBuffer: 50
                    }
                }
            }

            Repeater {
                model: modelRoot.links;
                Item {
                    id: item
                    property QtObject link: modelRoot.links.get(index);

                    property bool dragActive : from.Drag.active || to.Drag.active

                    property bool fromAttached : (link.from !== null)
                    property bool toAttached : (link.to !== null)

                    property double mfx: 10; //my from x
                    property double mfy: 10; //my from y
                    property double mtx: 20; //my to x
                    property double mty: 20; //my to y


                    // while attached
                    property double fx: (fromAttached? link.from.cpos.x : mfx);  //from x
                    property double fy: (fromAttached? link.from.cpos.y : mfy); //from y
                    property double tx: (toAttached? link.to.cpos.x : mtx); //to x
                    property double ty: (toAttached? link.to.cpos.y : mty); //to y


                    Behavior on fx { enabled: dragActive; NumberAnimation { duration: 100; easing.type: Easing.OutQuad; }}
                    Behavior on fy { enabled: dragActive;NumberAnimation { duration: 100; easing.type: Easing.OutQuad; }}
                    Behavior on tx { enabled: dragActive;NumberAnimation { duration: 100; easing.type: Easing.OutQuad; }}
                    Behavior on ty { enabled: dragActive;NumberAnimation { duration: 100; easing.type: Easing.OutQuad; }}

                    Arrow {
                        id: recta

                        x: Math.min(fx,tx) - 5
                        y: Math.min(fy,ty) - 5
                        width:  Math.abs(fx - tx) + 10
                        height: Math.abs(fy - ty) + 10
                    }

                    Rectangle {
                        id: from

                        radius: 5
                        opacity: (dragArea1.containsMouse || Drag.active ? 0.8: 0.01)
                        Behavior on opacity { NumberAnimation { duration: 200; easing.type: Easing.OutQuad; }}


                        x: (Drag.active? null : fx)
                        y: (Drag.active? null : fy)

                        width: 10
                        height: 10

                        parent: (Drag.active? canvas : item )

                        function setRect(i)
                        {
                            link.from = i;
                        }

                        color: (link.from === null ? "green" : "blue")
                        Behavior on color { ColorAnimation { duration: 200 } }

                        Drag.active: dragArea1.drag.active
                        Drag.hotSpot.x: 5
                        Drag.hotSpot.y: 5



                        MouseArea {
                            id: dragArea1
                            anchors.fill: parent
                            drag.target: parent
                            anchors.margins: -8
                            hoverEnabled: true
                            onPositionChanged: {
                                if (pressed)
                                {
                                    item.mfx = from.x
                                    item.mfy = from.y
                                }
                            }

                            onReleased: {
                                link.from = null
                                parent.Drag.drop()
                            }
                        }
                    }

                    Rectangle {
                        id: to


                        radius: 5

                        x: (Drag.active? null : tx)
                        y: (Drag.active? null : ty)

                        opacity: (dragArea2.containsMouse || Drag.active ? 0.8: 0.01)
                        Behavior on opacity { NumberAnimation { duration: 200; easing.type: Easing.OutQuad; }}

                        width: 10
                        height: 10

                        parent: (Drag.active? canvas : item )

                        function setRect(i)
                        {
                            link.to = i;
                        }

                        color: (link.to === null ? "yellow" : "blue")
                        Behavior on color { ColorAnimation { duration: 200 } }

                        Drag.active: dragArea2.drag.active
                        Drag.hotSpot.x: 5
                        Drag.hotSpot.y: 5

                        MouseArea {
                            id: dragArea2

                            anchors.fill: parent
                            drag.target: parent
                            anchors.margins: -8
                            hoverEnabled: true

                            onPositionChanged: {
                                if (pressed)
                                {
                                    item.mtx = to.x
                                    item.mty = to.y
                                }
                            }

                            onReleased: {
                                link.to = null
                                parent.Drag.drop()
                            }

                        }
                    }
                }
            }
        }
    }
    /*
    PinchArea {
        anchors.fill: canvas

        pinch.dragAxis:Pinch.XAndYAxis
        pinch.minimumScale: 0.2;
        pinch.maximumScale: 1.0;

        function update(pinch)
        {
            var pos = mapToItem(tmp, pinch.center.x, pinch.center.y);

            trans.origin.x = pos.x;
            trans.origin.y = pos.y;

            trans.xScale += pinch.scale - pinch.previousScale;
            trans.yScale = trans.xScale;

            //console.log(scale + " " + pos.x + " "+ pos.y)
        }

        onPinchStarted: update(pinch)
        onPinchUpdated: update(pinch)
        onPinchFinished: update(pinch)
    }
*/

    DropShadow {
        anchors.fill: canvas
        horizontalOffset: 1
        verticalOffset: 1
        radius: 4.0
        samples: 8
        color: "#80000000"
        source: canvas
    }

    Button {
        id: button2
        x: 0
        y: 0
        text: "+link"
        onClicked:  modelRoot.links.addRowWithUndo();
    }
}

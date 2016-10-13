import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls.Styles 1.0
import QtQuick.Dialogs 1.0
import QtQuick 2.1

ApplicationWindow {
//    x: root.model.winRect.x
//    y: root.model.winRect.y
//    width: root.model.winRect.width
//    height: root.model.winRect.height

//    onXChanged: root.model.winRect.x = x
//    onYChanged: root.model.winRect.y = y
//    onWidthChanged: root.model.winRect.width = width
//    onHeightChanged: root.model.winRect.height = height
    property QtObject modelRoot: null
    Component.onCompleted: modelRoot = modelLoader.create();

    width: 1200
    height: 600

    minimumWidth: 400
    minimumHeight: 300

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem { action: openAction; }
            MenuItem { action: saveAction; }

            MenuItem { action: burnAction; }

            MenuSeparator {}
            MenuItem { text: "Quit"; onTriggered: Qt.quit() }
        }
        Menu {
            title: "Edit"
            MenuItem { action: undoAction; }
            MenuItem { action: redoAction; }
            MenuSeparator {}
            MenuItem { action: diagramAction; }
        }

        Menu {
            title: "Help"
            MenuItem { text: "About..."; enabled: false }
        }
    }

    toolBar: ToolBar {
        RowLayout {
            ToolButton { action: openAction }
            ToolButton { action:saveAction }

            ToolButton { action: undoAction }
            ToolButton { action: redoAction }

            ToolButton { action: diagramAction; }
            ToolButton { action: burnAction }

        }
    }
    Action {
        id: openAction
        text: "Open project..."
        shortcut: "Ctrl+O"
        iconSource: "qrc:/resources/folder.png"
        onTriggered: openDialog.open()
        tooltip: text
    }
    Action {
        id: saveAction
        text: "Save project..."
        shortcut: "Ctrl+S"
        iconSource: "qrc:/resources/save.png"
        onTriggered: saveDialog.open()
        tooltip: text
    }
    Action {
        id: undoAction
        text: "Undo " + modelLoader.undoer().undoText
        shortcut: "Ctrl+Z"
        iconSource: "qrc:/resources/undo.png"
        onTriggered: modelLoader.undoer().undo()
        tooltip: text
    }
    Action {
        id: redoAction
        text: "Redo " + modelLoader.undoer().redoText
        shortcut: "Ctrl+Shift+Z"
        iconSource: "qrc:/resources/redo.png"
        onTriggered: modelLoader.undoer().redo()
        tooltip: text
    }
    Action {
        id: burnAction
        text: "Burn..."
        shortcut: "Ctrl+B"
        iconSource: "qrc:/resources/burn.png"
        onTriggered: fileDialog.open()
        tooltip: text
    }
    Action {
        id: diagramAction
        text: "Show diagram..."
        shortcut: ""
        iconSource: "qrc:/resources/diagram.png"
        checkable: true
        onCheckedChanged: win.state = (checked?"diagram":"")
        tooltip: text
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a folder where files will be generated..."

        selectMultiple: false
        selectFolder: true

        onAccepted: modelRoot.generateFiles(fileDialog.fileUrl);
    }

    FileDialog {
        id: openDialog
        title: "Open project file..."

        selectMultiple: false
        selectFolder: false
        selectExisting: true

        nameFilters: [ "Coder project (*.cod)" ]

        onAccepted: modelRoot = modelLoader.load(fileUrl);
    }

    FileDialog {
        id: saveDialog
        title: "Save project file..."

        selectMultiple: false
        selectFolder: false
        selectExisting: false

       nameFilters: [ "Coder project (*.cod)" ]

        onAccepted: modelLoader.save(fileUrl, modelRoot)
    }

    Item {
        id: win
        anchors.fill: parent

        SplitView {
            id: splitview
            anchors.fill: parent

            visible: opacity != 0.0;
            opacity: 1.0;

            ClassList {
                id: classList

                Layout.minimumWidth: 100

                model: modelRoot

                onSelectedIndexChanged: win.state = "prop"

                // do not bind as layout takes care of width
                Component.onCompleted: width = 200
            }
            SplitView {
                height: parent.height

                orientation: Qt.Vertical
                Layout.minimumWidth: 50
                Layout.fillWidth: true
                ClassEditor {
                    id: classEditor
                    Layout.minimumHeight: 50
                    Layout.fillWidth: true

                    classObj: modelRoot.classes.get(classList.selectedIndex)
                }
                PropertyEditor {
                    id: propertyEditor

                    opacity: (properties !== null? 1.0 : 0.0)
                    visible: opacity != 0.0

                    Layout.fillWidth: true
                    properties: classEditor.classObj ? classEditor.classObj.properties : null
                }
            }
        }
        Diagram {
            id: diagram

            visible: opacity != 0.0;
            opacity: 0.0;

            anchors.fill: parent
        }


        states: State {
            name: "diagram";
            PropertyChanges { target: splitview; opacity: 0.0; }
            PropertyChanges { target: diagram; opacity: 1.0; }
        }
        transitions: Transition {
            from: ""; to: "diagram"; reversible: true
            ParallelAnimation {
                NumberAnimation { properties: "opacity"; duration: 500; easing.type: Easing.InOutQuad }
            }
        }
    }
}

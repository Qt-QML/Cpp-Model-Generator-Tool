import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1
import QtQuick.Controls.Styles 1.3
import QtQuick.Dialogs 1.2
import QtQuick 2.4
import Model 1.0

ApplicationWindow {
//    x: root.model.winRect.x
//    y: root.model.winRect.y
//    width: root.model.winRect.width
//    height: root.model.winRect.height

//    onXChanged: root.model.winRect.x = x
//    onYChanged: root.model.winRect.y = y
//    onWidthChanged: root.model.winRect.width = width
//    onHeightChanged: root.model.winRect.height = height
    property QtObject modelRoot
    Component.onCompleted: modelRoot = ModelLoader.create();

    width: 1200
    height: 600

    minimumWidth: 400
    minimumHeight: 300

    menuBar: MenuBar {
        Menu {
            title: "File"
            MenuItem { action: newAction; }
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
            ToolButton { action: saveAction }

            ToolButton { action: undoAction }
            ToolButton { action: redoAction }

            ToolButton { action: diagramAction; }
            ToolButton { action: burnAction }

        }
    }
    Action {
        id: newAction
        text: "New Project"
        iconName: "file-new"
        onTriggered: modelRoot = ModelLoader.create();
    }
    Action {
        id: openAction
        text: "Open Project..."
        shortcut: StandardKey.Open
        iconSource: "qrc:/resources/folder.png"
        onTriggered: openDialog.open()
    }
    Action {
        id: saveAction
        text: "Save Project..."
        shortcut: StandardKey.Save
        iconSource: "qrc:/resources/save.png"
        onTriggered: saveDialog.open()
    }
    Action {
        id: undoAction
        text: "Undo " + ModelLoader.undoer().undoText
        shortcut: StandardKey.Undo
        iconSource: "qrc:/resources/undo.png"
        onTriggered: ModelLoader.undoer().undo()
    }
    Action {
        id: redoAction
        text: "Redo " + ModelLoader.undoer().redoText
        shortcut: StandardKey.Redo
        iconSource: "qrc:/resources/redo.png"
        onTriggered: ModelLoader.undoer().redo()
    }
    Action {
        id: burnAction
        text: "Burn..."
        shortcut: "Ctrl+B"
        iconSource: "qrc:/resources/burn.png"
        onTriggered: fileDialog.open()
    }
    Action {
        id: diagramAction
        text: "Show Diagram"
        iconSource: "qrc:/resources/diagram.png"
        checkable: true
        onCheckedChanged: win.state = (checked?"diagram":"")
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a folder where files will be generated..."

        selectMultiple: false
        selectFolder: true

        onAccepted: codeGenerator.generateFiles(modelRoot, fileDialog.fileUrl);
    }

    FileDialog {
        id: openDialog
        title: "Open Project File..."

        nameFilters: [ "Coder projects (*.cod *.json)" ]

        onAccepted: {
            var urlString = fileUrl.toString();
            if (urlString.substring(urlString.lastIndexOf(".") + 1) === "cod")
                modelRoot = ModelLoader.load(fileUrl);
            else
                modelRoot = ModelLoader.loadFromJson(fileUrl);
        }
    }

    FileDialog {
        id: saveDialog
        title: "Save Project File..."

        selectExisting: false

        nameFilters: [
            "Coder project (*.cod)",
            "JSON file (*.json)"
        ]

        onAccepted: {
            if (selectedNameFilter === "Coder project (*.cod)")
                ModelLoader.save(fileUrl, modelRoot)
            else
                ModelLoader.saveAsJson(fileUrl, modelRoot)
        }
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

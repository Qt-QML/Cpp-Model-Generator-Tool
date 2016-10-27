import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    id: splitview2

    property QtObject model
    property alias selectedIndex: classesTable.currentRow

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: rootClass

            placeholderText: "RootClassName"

            text: splitview2.model.name
            onTextChanged: splitview2.model.name = text
        }

        TableView {
            id: classesTable

            Layout.fillWidth: true
            Layout.fillHeight: true

            frameVisible: false
            highlightOnFocus: false
            model: splitview2.model.classes

            itemDelegate: EditableTextDelegate { view: classesTable }

            TableViewColumn { title: "Classes"; role: "row" }
        }

        RowLayout {
            Button {
                text: " + "
                onClicked: splitview2.model.classes.addRowWithUndo()
            }
            Button {
                text: " - "
                onClicked: splitview2.model.classes.removeWithUndo(selectedIndex)
            }
        }
    }
}

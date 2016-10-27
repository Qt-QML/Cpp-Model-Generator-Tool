import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

SplitView {
    property QtObject enums
    property QtObject enumObject: enums ? enums.get(enumsTable.currentRow) : null
    property QtObject enumValues: enumObject ? enumObject.values : null

    ColumnLayout {
        width: 200

        TableView {
            id: enumsTable

            Layout.fillHeight: true
            frameVisible: false
            highlightOnFocus: false
            model: enums

            itemDelegate: EditableTextDelegate { view: enumsTable }

            TableViewColumn { title: "Enums"; role: "row" }
        }

        RowLayout {
            Button {
                text: " + "
                enabled: enums
                onClicked: enums.addRowWithUndo()
            }
            Button {
                text: " - "
                enabled: enums && enumsTable.currentRow != -1
                onClicked: enums.removeWithUndo(enumsTable.currentRow)
            }
        }
    }

    ColumnLayout {
        Layout.fillWidth: true
        TableView {
            id: enumValuesTable

            Layout.fillHeight: true
            Layout.fillWidth: true
            frameVisible: false
            highlightOnFocus: false
            model: enumValues

            itemDelegate: EditableTextDelegate { view: enumValuesTable }

            TableViewColumn { title: "Enum Values"; role: "row" }
        }

        RowLayout {
            Button {
                text: " + "
                enabled: enumValues
                onClicked: enumValues.addRowWithUndo()
            }
            Button {
                text: " - "
                enabled: enumValues && enumValuesTable.currentRow != -1
                onClicked: enumValues.removeWithUndo(enumValuesTable.currentRow)
            }
        }
    }
}

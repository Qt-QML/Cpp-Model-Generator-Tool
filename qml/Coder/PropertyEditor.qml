import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

SplitView {
    property QtObject properties
    property alias selectedIndex: propertiesTable.currentRow

    ColumnLayout {
        width: 200
        height: parent.height
        TableView {
            id: propertiesTable

            Layout.fillHeight: true
            frameVisible: false
            highlightOnFocus: false
            model: properties

            itemDelegate: EditableTextDelegate { view: propertiesTable }

            TableViewColumn { title: "Properties"; role: "row" }
        }

        RowLayout {
            Button {
                text: " + "
                onClicked: properties.addRowWithUndo()
            }
            Button {
                text: " - "
                onClicked: properties.removeWithUndo(selectedIndex)
            }
        }
    }

    PropView {
        id: propView

        opacity: (selectedProp !== null? 1.0 : 0.0)
        visible: opacity != 0.0

        selectedProp: selectedIndex >= 0 ? properties.get(selectedIndex) : null

        Behavior on opacity { PropertyAnimation { duration: 200 }}
    }
}

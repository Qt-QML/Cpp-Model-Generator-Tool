import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

SplitView {
    id: splitview1

    property QtObject properties
    property int selectedIndex: tableList.currentRow


    ColumnLayout {
        width: 200
        height: parent.height
        TableView {
            id: tableList

            Layout.fillHeight: true
            frameVisible: false
            highlightOnFocus: false
            model: properties

            itemDelegate: Text {
                anchors.verticalCenter: parent.verticalCenter
                text: properties.get(styleData.row).name
            }

            TableViewColumn { title: "Properties" }
        }
        RowLayout {
            Button {
                id: button1
                x: 45
                y: 379
                text: "+"
                onClicked: properties.addRowWithUndo()
            }
            Button {
                id: button2
                x: 45
                y: 405
                text: "-"
                onClicked: properties.removeWithUndo(tableList.currentRow)
            }
        }
    }
    PropView {
        id: propView

        opacity: (selectedProp !== null? 1.0 : 0.0)
        visible: opacity != 0.0

        selectedProp: (properties.count > 0 ? properties.get(selectedIndex) : null)

        Behavior on opacity { PropertyAnimation { duration: 200 }}
    }
}

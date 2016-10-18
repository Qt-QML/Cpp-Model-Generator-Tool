import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Item {
    id: splitview2

    property QtObject model
    property alias selectedIndex: tableList.currentRow

    ColumnLayout {
        anchors.fill: parent

        TextField {
            id: rootClass

            placeholderText: "RootClassName"

            text: splitview2.model.name
            onTextChanged: splitview2.model.name = text
        }
        TableView {
            id: tableList

            Layout.fillWidth: true
            Layout.fillHeight: true

            frameVisible: false
            highlightOnFocus: false
            model: splitview2.model.classes

            itemDelegate: Text {
                anchors.verticalCenter: parent.verticalCenter
                text: styleData.value ? styleData.value.name : "";
                color: styleData.textColor
            }

            TableViewColumn { title: "Classes"; role: "row" }
        }
        RowLayout {
            Button {
                id: button1
                x: 45
                y: 379
                text: " + "
                onClicked: splitview2.model.classes.addRowWithUndo()
            }

            Button {
                id: button2
                x: 45
                y: 405
                text: " - "
                onClicked: splitview2.model.classes.removeWithUndo(selectedIndex)
            }
        }
    }
}

import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Text {
    property TableView view;

    anchors.verticalCenter: parent ? parent.verticalCenter : undefined
    text: styleData.value ? styleData.value.name : "";
    color: styleData.textColor

    function rename() {
        textInput.visible = true
        textInput.text = text
        textInput.forceActiveFocus()
        textInput.selectAll()
    }

    MouseArea {
        anchors.fill: parent
        onPressed: {
            if (view) {
                view.forceActiveFocus(Qt.MouseFocusReason)

                // Nothing appears to happen automatically...
                view.selection.clear()
                view.selection.select(styleData.row)
                view.currentRow = styleData.row
            }
        }
        onDoubleClicked: rename()
    }

    TextField {
        id: textInput
        anchors.fill: parent
        visible: false
        onEditingFinished: {
            if (text.length > 0)
                styleData.value.name = text
            visible = false
        }
    }

    Keys.onPressed: {
        if (event.key === Qt.Key_F2)
            rename()
    }
}

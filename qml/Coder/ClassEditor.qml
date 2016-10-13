import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Item {
    property QtObject classObj

    TextField {
        id: text_field1
        x: 91
        y: 15
        placeholderText: ""

        text: classObj ? classObj.name : ""
        onTextChanged: classObj.name = text
    }

    Text {
        id: text1
        x: 13
        y: 18
        text: qsTr("Class name:")
        font.pixelSize: 12
    }

/*
    CheckBox {
        id: check_box1
        x: 250
        y: 16
        text: "index"

        checked: classObj && classObj.index
        onClicked: {
            classObj.index = checked
            checked = Qt.binding(function() { return classObj && classObj.index; })
        }
    }
    */
}

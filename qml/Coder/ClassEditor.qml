import QtQuick 2.0
import QtQuick.Controls 1.0
import QtQuick.Layouts 1.0

Item {
    property QtObject classObj

    // workaround
    onClassObjChanged: {
        //check_box1.checked = classObj.index;
    }

    TextField {
        id: text_field1
        x: 91
        y: 15
        placeholderText: ""

        text: classObj.name
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

        checkedState: (classObj.index? Qt.Checked : Qt.Unchecked)
        onClicked: classObj.index = checked
    }
    */
}

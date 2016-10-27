import QtQuick 2.4
import QtQuick.Controls 1.3
import QtQuick.Layouts 1.1

Item {
    property QtObject selectedProp

    function updateInitAndDestruct() {
        if (!selectedProp)
            return;

        var type = selectedProp.type;
        var name = selectedProp.name;
        var subType = selectedProp.subType;

        if (type === "QObject*")
        {
            if (selectedProp.null)
            {
                selectedProp.init = "_" + name + " = Q_NULLPTR";
                selectedProp.destruct = "";
            }
            else
            {
                var Name = name[0].toUpperCase() + name.substring(1);

                selectedProp.init =
                        "_" + name + " = Q_NULLPTR;\n" +
                        "    set" + Name + "Imp(new " + subType + ")";
                selectedProp.destruct = "_" + name + "->deleteLater()";
            }
        }

        if (type === "ObjectList*")
        {
            var count = selectedProp.count;

            selectedProp.init = ("_" + name + " = new ObjectList(" + subType + "::staticMetaObject, %1)").arg(count);
            selectedProp.destruct = "_" + name + "->deleteLater()";
        }
    }

    Connections {
        target: selectedProp
        onNullChanged: updateInitAndDestruct()
        onNameChanged: updateInitAndDestruct()
        onTypeChanged: updateInitAndDestruct()
        onSubTypeChanged: updateInitAndDestruct()
        onCountChanged: updateInitAndDestruct()
    }

    TextField {
        id: name
        x: 68
        y: 16
        width: 162
        height: 21
        placeholderText: "Name"

        text: selectedProp ? selectedProp.name : ""
        onTextChanged: if (selectedProp) selectedProp.name = text
    }

    ComboBox {
        id: type
        x: 68
        y: 47
        width: 110

        property string currentType: "QString"


        model: ListModel {
            id: items
            ListElement { text: "" ; sub: false; }
            ListElement { text: "QString" ; sub: false; }
            ListElement { text: "bool" ;    sub: false; }
            ListElement { text: "QObject*"; sub: true; }
            ListElement { text: "ObjectList*"; sub: true;}
            ListElement { text: "enum"; sub: true;}
            ListElement { text: "double" ; sub: false; }
            ListElement { text: "float" ; sub: false; }
            ListElement { text: "qint32" ; sub: false; }
            ListElement { text: "quint32" ; sub: false; }
            ListElement { text: "qint16" ; sub: false; }
            ListElement { text: "quint16" ; sub: false; }
            ListElement { text: "qint8" ; sub: false; }
            ListElement { text: "quint8" ; sub: false; }
            ListElement { text: "QColor" ; sub: false; }
            ListElement { text: "QPointF" ; sub: false; }
            ListElement { text: "QRectF" ; sub: false; }
            ListElement { text: "QImage" ; sub: false; }
            ListElement { text: "Song*" ; sub: false; }
        }

        Binding {
            target: type;
            property: "currentType";
            value: selectedProp ? selectedProp.type : "";
        }
        onCurrentTypeChanged: {
            for (var i=0;i<items.count;i++)
                if (items.get(i).text === currentType)
                {
                    currentIndex = i;
                    break;
                }
        }
        onCurrentIndexChanged: {
            if (selectedProp)
                selectedProp.type = items.get(currentIndex).text;
        }
    }
/*
    ComboBox {
        id: subType
        x: 193
        y: 47
//fixme v5ib juhtuda kui propi vahetada ja uuel ei ole valitud, siis j22b combo uuendamata
        property string currentType: "QString"

        model: modelRoot.classes
        textRole: "name"

        opacity: type.model.get(type.currentIndex).sub
        visible: opacity

        onCurrentTypeChanged: {
            for (var i=0;i<modelRoot.classes.count;i++)
                if (modelRoot.classes.get(i).name === currentType)
                {
                    currentIndex = i;
                    break;
                }
        }

        Binding { target:subType; property: "currentType"; value: selectedProp.subType; }
        onCurrentIndexChanged:  { if (modelRoot.classes!== null && modelRoot.classes.get(currentIndex) !== null) selectedProp.subType = modelRoot.classes.get(currentIndex).name; }
    }
*/
    TextField {
        id: subType
        x: 193
        y: 47

        visible: type.currentType === "ObjectList*" || type.currentType === "QObject*" || type.currentType === "enum"

        placeholderText: {
            if (type.currentType === "enum")
                return "enum type";
            else
                return "ptr type";
        }

        text: selectedProp ? selectedProp.subType: "";
        onTextChanged: if (selectedProp) selectedProp.subType = text
    }

    TextField {
        id: typeCount

        visible: type.currentType === "ObjectList*"

        x: 193+110
        y: 47

        placeholderText: "init count"
        text: selectedProp ? selectedProp.count : "";
        onTextChanged: if (selectedProp) selectedProp.count = text
    }

    CheckBox {
        id: typeNull

        visible: type.currentType === "QObject*"

        x: 193+110
        y: 47

        text: "isNull";

        checked: selectedProp && selectedProp.null
        onClicked: {
            selectedProp.null = checked
            checked = Qt.binding(function() { return selectedProp && selectedProp.null; })
        }
    }



    TextField {
        id: init

        readOnly: type.currentType === "ObjectList*" || type.currentType === "QObject*"
        textColor: readOnly ? "gray" : "black"

        x: 68
        y: 109
        width: 375
        height: 21
        placeholderText: "_name = \"untitled\"";

        text: selectedProp ? selectedProp.init : ""
        onTextChanged: if (selectedProp) selectedProp.init = text
    }

    TextField {
        id: validate
        x: 68
        y: 78
        width: 375
        height: 21
        placeholderText: "val.contains(\"a\")"

        text: selectedProp ? selectedProp.validate : ""
        onTextChanged: selectedProp.validate = text

    }

    TextField {
        id: destruct

        readOnly: type.currentType === "ObjectList*" || type.currentType === "QObject*"
        textColor : readOnly ? "gray": "black"

        x: 68
        y: 141
        width: 375
        height: 21
        placeholderText: "Text Field"

        text: selectedProp ? selectedProp.destruct : ""
        onTextChanged: if (selectedProp) selectedProp.destruct = text
    }

    CheckBox {
        id: write
        x: 457
        y: 34
        text: "Write"

        checked: selectedProp && selectedProp.write
        onClicked: {
            selectedProp.write = checked
            checked = Qt.binding(function() { return selectedProp && selectedProp.write; })
        }
    }

    CheckBox {
        id: read
        x: 457
        y: 14
        text: "Read"

        checked: selectedProp && selectedProp.read
        onClicked: {
            selectedProp.read = checked
            checked = Qt.binding(function() { return selectedProp && selectedProp.read; })
        }
    }

    CheckBox {
        id: save
        x: 457
        y: 94
        text: "Save"

        checked: selectedProp && selectedProp.save
        onClicked: {
            selectedProp.save = checked
            checked = Qt.binding(function() { return selectedProp && selectedProp.save; })
        }
    }

    CheckBox {
        id: notify
        x: 457
        y: 54
        text: "Notify"

        visible: write.checked

        checked: selectedProp && selectedProp.notify
        onClicked: {
            selectedProp.notify = checked
            checked = Qt.binding(function() { return selectedProp && selectedProp.notify; })
        }
    }

    CheckBox {
        id: undo
        x: 457
        y: 74
        text: "Undo"

        visible: write.checked

        checked: selectedProp && selectedProp.undo
        onClicked: {
            selectedProp.undo = checked
            checked = Qt.binding(function() { return selectedProp && selectedProp.undo; })
        }
    }

    Text {
        id: text1
        x: 24
        y: 19
        text: qsTr("Name")
        font.pixelSize: 12
    }

    Text {
        id: text2
        x: 24
        y: 50
        width: 33
        height: 15
        text: qsTr("Type")
        font.pixelSize: 12
    }
    Text {
        id: text5
        x: 38
        y: 112
        text: qsTr("Init")
        font.pixelSize: 12
    }
    Text {
        id: text4
        x: 10
        y: 81
        text: qsTr("Validate")
        font.pixelSize: 12
    }

    Text {
        id: text3
        x: 8
        y: 144
        text: qsTr("Destruct")
        font.pixelSize: 12
    }
}

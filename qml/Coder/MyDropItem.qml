import QtQuick 2.4

DropArea {
    id: snaptarget

    property QtObject obj: null

    property QtObject source: null;

    function upd()
    {
        var canvasPos = mapToItem(canvas, width/2.0, height/2.0);

        if (obj !== null)
            obj.cpos = Qt.point(canvasPos.x, canvasPos.y)
    }

    onXChanged: upd()
    onYChanged: upd()

    Connections {
        target: parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }
    Connections {
        target: parent.parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }
    Connections {
        target: parent.parent.parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }
    Connections {
        target: parent.parent.parent.parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }
    Connections {
        target: parent.parent.parent.parent.parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }
    Connections {
        target: parent.parent.parent.parent.parent.parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }
    Connections {
        target: parent.parent.parent.parent.parent.parent.parent;
        onXChanged: snaptarget.upd();
        onYChanged: snaptarget.upd();
    }

    onEntered: { source = drag.source; source.setRect(obj); }
    onDropped: { drag.source.setRect(obj); }
    onExited:  { source.setRect(null); }
}

#include <QtWidgets/QApplication>
#include <QtQml>
#include <QtQuick/QQuickView>

#include "qtquick2applicationviewer.h"
#include "model/root.h"
#include "model_engine/undoer.h"
#include "arrow.h"
#include "model/modelloader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlEngine engine;
    QQmlComponent component(&engine);

    qmlRegisterType<Arrow>("CustomCPP", 1, 0, "Arrow"); //%%%

    QQmlContext *ctxt = engine.rootContext();

    ctxt->setContextProperty("modelLoader", new ModelLoader);

    component.loadUrl(QUrl("../Resources/qml/Coder/main.qml"));
    if ( !component.isReady() ) {
        qWarning("%s", qPrintable(component.errorString()));
        return -1;
    }
    QObject *topLevel = component.create();
    QQuickWindow *window = qobject_cast<QQuickWindow *>(topLevel);
    if ( !window ) {
        qWarning("Error: Your root item has to be a Window.");
        return -1;
    }
    QObject::connect(&engine, SIGNAL(quit()), &app, SLOT(quit()));
    window->show();

    return app.exec();
}

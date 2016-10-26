#include <QtWidgets/QApplication>
#include <QtQml>
#include <QtQuick/QQuickView>

#include "model_engine/undoer.h"
#include "arrow.h"
#include "codegenerator.h"
#include "model/modelloader.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QQmlEngine engine;
    QQmlComponent component(&engine);

    qmlRegisterType<Arrow>("CustomCPP", 1, 0, "Arrow"); //%%%

    QQmlContext *context = engine.rootContext();

    ModelLoader::registerTypes();

    context->setContextProperty("codeGenerator", new CodeGenerator);

#ifdef Q_OS_MAC
    component.loadUrl(QUrl("../Resources/qml/Coder/main.qml"));
#else
    component.loadUrl(QUrl("qml/Coder/main.qml"));
#endif
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

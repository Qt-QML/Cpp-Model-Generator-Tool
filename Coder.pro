# Add more folders to ship with the application, here
folder_01.source = qml/Coder
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

QT += qml quick widgets

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

CONFIG += c++11

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    model/classmodel.cpp \
    model/model.cpp \
    model/classprop.cpp \
    model_engine/objectlist.cpp \
    model_engine/undoer.cpp \
    model_engine/undocommands.cpp \
    model/links.cpp \
    arrow.cpp \
    codegenerator.cpp \
    model/modelloader.cpp

# Installation path
# target.path =

# Please do not modify the following two lines. Required for deployment.
include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
qtcAddDeployment()

HEADERS += \
    model/classes.h \
    model/classmodel.h \
    model/classprop.h \
    model/links.h \
    model/model.h \
    model/modelloader.h \
    model_engine/jsonconverters.h \
    model_engine/objectlist.h \
    model_engine/undocommands.h \
    model_engine/undoer.h \
    arrow.h \
    codegenerator.h

OTHER_FILES += \
    templates/class.cpp \
    templates/class.h

RESOURCES += \
    resources.qrc

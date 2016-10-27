#include "modelloader.h"
#include "classes.h"

#include <QDebug>
#include <QtQml>
#include <QQmlEngine>
#include <QJsonDocument>
#include <QSaveFile>
#include <QUrl>

static QObject *modelLoaderProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new ModelLoader;
}

void ModelLoader::registerTypes()
{
    // singleton
    qmlRegisterSingletonType<ModelLoader>("Model", 1, 0, "ModelLoader", modelLoaderProvider);

    // object
    qmlRegisterUncreatableType<ObjectList>("Model", 1, 0, "ObjectList", "ObjectList is created natively");
    qmlRegisterUncreatableType<Undoer>("Model", 1, 0, "Undoer", "Undoer is created natively");

    // all other classes that make up the model
    qmlRegisterUncreatableType<Model>("Model", 1, 0, "Model", "Model is created natively");
    qmlRegisterUncreatableType<ClassModel>("Model", 1, 0, "ClassModel", "ClassModel is created natively");
    qmlRegisterUncreatableType<ClassProp>("Model", 1, 0, "ClassProp", "ClassProp is created natively");
    qmlRegisterUncreatableType<Links>("Model", 1, 0, "Links", "Links is created natively");
    qmlRegisterUncreatableType<Enum>("Model", 1, 0, "Enum", "Enum is created natively");
    qmlRegisterUncreatableType<EnumValue>("Model", 1, 0, "EnumValue", "EnumValue is created natively");

}

ModelLoader::ModelLoader(QObject *parent) :
    QObject(parent)
{
}

Undoer *ModelLoader::undoer() const
{
    return Undoer::instance();
}

Model *ModelLoader::create()
{
    return new Model;
}

Model *ModelLoader::load(const QString &fileName)
{
    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (!f.open(QIODevice::ReadOnly))
        return nullptr;

    Model *ret = new Model;

    quint32 count;
    QDataStream ds(&f);

    // all objects must be created before setting values and linking pointers
    ds >> count;
    ds >> count; ClassModel::init(count);
    ds >> count; ClassProp::init(count);
    ds >> count; Links::init(count);
    ds >> count; Enum::init(count);
    ds >> count; EnumValue::init(count);

    qDebug() << "loading ClassModel: " << ClassModel::_ptrs.count();
    qDebug() << "loading ClassProp: " << ClassProp::_ptrs.count();
    qDebug() << "loading Links: " << Links::_ptrs.count();
    qDebug() << "loading Enum: " << Enum::_ptrs.count();
    qDebug() << "loading EnumValue: " << EnumValue::_ptrs.count();

    // load
    ds >> ret;
    ClassModel::load(ds);
    ClassProp::load(ds);
    Links::load(ds);
    Enum::load(ds);
    EnumValue::load(ds);

    return ret;
}

bool ModelLoader::save(const QString &fileName, QObject *model) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (!f.open(QIODevice::WriteOnly))
        return false;

    createIndices();

    QDataStream ds(&f);

    // save each class separately
    ds << (quint32)1;
    ds << (quint32)ClassModel::_ptrs.count();
    ds << (quint32)ClassProp::_ptrs.count();
    ds << (quint32)Links::_ptrs.count();
    ds << (quint32)Enum::_ptrs.count();
    ds << (quint32)EnumValue::_ptrs.count();

    ds << static_cast<Model*>(model);
    ClassModel::save(ds);
    ClassProp::save(ds);
    Links::save(ds);
    Enum::save(ds);
    EnumValue::save(ds);

    clearIndices();

    return f.commit();
}

Model *ModelLoader::loadFromJson(const QString &fileName)
{
    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (!f.open(QIODevice::ReadOnly))
        return nullptr;

    const QByteArray json = f.readAll();
    QJsonParseError error;
    const QJsonObject rootObject = QJsonDocument::fromJson(json, &error).object();
    if (error.error != QJsonParseError::NoError)
        return nullptr;

    Model *ret = new Model;

    // all objects must be created before setting values and linking pointers
    ClassModel::init(rootObject["ClassModel"].toArray().size());
    ClassProp::init(rootObject["ClassProp"].toArray().size());
    Links::init(rootObject["Links"].toArray().size());
    Enum::init(rootObject["Enum"].toArray().size());
    EnumValue::init(rootObject["EnumValue"].toArray().size());

    qDebug() << "loading ClassModel: " << ClassModel::_ptrs.count();
    qDebug() << "loading ClassProp: " << ClassProp::_ptrs.count();
    qDebug() << "loading Links: " << Links::_ptrs.count();
    qDebug() << "loading Enum: " << Enum::_ptrs.count();
    qDebug() << "loading EnumValue: " << EnumValue::_ptrs.count();

    // load
    fromJson(rootObject[QLatin1String("model")], ret);
    ClassModel::loadFromJson(rootObject[QLatin1String("ClassModel")].toArray());
    ClassProp::loadFromJson(rootObject[QLatin1String("ClassProp")].toArray());
    Links::loadFromJson(rootObject[QLatin1String("Links")].toArray());
    Enum::loadFromJson(rootObject[QLatin1String("Enum")].toArray());
    EnumValue::loadFromJson(rootObject[QLatin1String("EnumValue")].toArray());

    return ret;
}

bool ModelLoader::saveAsJson(const QString &fileName, QObject *model) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (!f.open(QIODevice::WriteOnly))
        return false;

    createIndices();

    QJsonObject rootObject;

    // save each class separately
    rootObject.insert(QLatin1String("model"), toJson(static_cast<Model*>(model)));
    rootObject.insert(QLatin1String("ClassModel"), ClassModel::saveToJson());
    rootObject.insert(QLatin1String("ClassProp"), ClassProp::saveToJson());
    rootObject.insert(QLatin1String("Links"), Links::saveToJson());
    rootObject.insert(QLatin1String("Enum"), Enum::saveToJson());
    rootObject.insert(QLatin1String("EnumValue"), EnumValue::saveToJson());

    clearIndices();

    f.write(QJsonDocument(rootObject).toJson());

    return f.commit();
}

void ModelLoader::createIndices() const
{
    //create indices
    ClassModel::createIndex();
    ClassProp::createIndex();
    Links::createIndex();
    Enum::createIndex();
    EnumValue::createIndex();

    qDebug() << "saving ClassModel: " << ClassModel::_ptrs.count();
    qDebug() << "saving ClassProp: " << ClassProp::_ptrs.count();
    qDebug() << "saving Links: " << Links::_ptrs.count();
    qDebug() << "saving Enum: " << Enum::_ptrs.count();
    qDebug() << "saving EnumValue: " << EnumValue::_ptrs.count();

}

void ModelLoader::clearIndices() const
{
    // free up memory
    ClassModel::clearIndex();
    ClassProp::clearIndex();
    Links::clearIndex();
    Enum::clearIndex();
    EnumValue::clearIndex();

}

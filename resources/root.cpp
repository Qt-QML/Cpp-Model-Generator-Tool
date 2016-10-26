#include "%%classname%%loader.h"
#include "classes.h"

#include <QDebug>
#include <QtQml>
#include <QQmlEngine>
#include <QJsonDocument>
#include <QSaveFile>
#include <QUrl>

static QObject *%%classname%%LoaderProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)

    return new %%Classname%%Loader;
}

void %%Classname%%Loader::registerTypes()
{
    // singleton
    qmlRegisterSingletonType<%%Classname%%Loader>("%%Classname%%", 1, 0, "%%Classname%%Loader", %%classname%%LoaderProvider);

    // object
    qmlRegisterUncreatableType<ObjectList>("%%Classname%%", 1, 0, "ObjectList", "ObjectList is created natively");
    qmlRegisterUncreatableType<Undoer>("%%Classname%%", 1, 0, "Undoer", "Undoer is created natively");

    // all other classes that make up the model
    qmlRegisterUncreatableType<%%Classname%%>("%%Classname%%", 1, 0, "%%Classname%%", "%%Classname%% is created natively");
%%register_types%%
}

%%Classname%%Loader::%%Classname%%Loader(QObject *parent) :
    QObject(parent)
{
}

Undoer *%%Classname%%Loader::undoer() const
{
    return Undoer::instance();
}

%%Classname%% *%%Classname%%Loader::create()
{
    return new %%Classname%%;
}

%%Classname%% *%%Classname%%Loader::load(const QString &fileName)
{
    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (!f.open(QIODevice::ReadOnly))
        return nullptr;

    %%Classname%% *ret = new %%Classname%%;

    quint32 count;
    QDataStream ds(&f);

    // all objects must be created before setting values and linking pointers
    ds >> count;
%%load_init%%
%%load_dbg_count%%
    // load
    ds >> ret;
%%load%%
    return ret;
}

bool %%Classname%%Loader::save(const QString &fileName, QObject *model) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (!f.open(QIODevice::WriteOnly))
        return false;

    createIndices();

    QDataStream ds(&f);

    // save each class separately
    ds << (quint32)1;
%%save_init%%
    ds << static_cast<%%Classname%%*>(model);
%%save%%
    clearIndices();

    return f.commit();
}

%%Classname%% *%%Classname%%Loader::loadFromJson(const QString &fileName)
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

    %%Classname%% *ret = new %%Classname%%;

    // all objects must be created before setting values and linking pointers
%%load_init_json%%
%%load_dbg_count%%
    // load
    fromJson(rootObject[QLatin1String("%%classname%%")], ret);
%%load_json%%
    return ret;
}

bool %%Classname%%Loader::saveAsJson(const QString &fileName, QObject *model) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (!f.open(QIODevice::WriteOnly))
        return false;

    createIndices();

    QJsonObject rootObject;

    // save each class separately
    rootObject.insert(QLatin1String("%%classname%%"), toJson(static_cast<%%Classname%%*>(model)));
%%save_json%%
    clearIndices();

    f.write(QJsonDocument(rootObject).toJson());

    return f.commit();
}

void %%Classname%%Loader::createIndices() const
{
    //create indices
%%save_index%%
%%save_dbg_count%%
}

void %%Classname%%Loader::clearIndices() const
{
    // free up memory
%%save_clear%%
}

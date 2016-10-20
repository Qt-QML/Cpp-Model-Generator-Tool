#include "%%classname%%loader.h"
#include "classes.h"

#include <QDebug>
#include <QtQml>
#include <QQmlEngine>
#include <QSaveFile>
#include <QUrl>

%%Classname%%Loader::%%Classname%%Loader(QObject *parent) :
    QObject(parent)
{
    // singleton
    qmlRegisterType<%%Classname%%>();

    // object
    qmlRegisterType<ObjectList>();
    qmlRegisterType<Undoer>();

    // all other classes that make up the model
%%register_types%%
}

Undoer * %%Classname%%Loader::undoer() const
{
    return Undoer::instance();
}

QObject * %%Classname%%Loader::create()
{
    return new %%Classname%%;
}

QObject * %%Classname%%Loader::load(const QString & fileName)
{
    %%Classname%% *ret = new %%Classname%%;

    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (f.open(QIODevice::ReadOnly))
    {
        quint32 count;
        QDataStream ds(&f);

        // all objects must be created before setting values and linking pointers
        ds >> count;
%%load_init%%
%%load_dbg_count%%
        // load
        ds >> ret;
%%load%%
    }

    return ret;
}

void %%Classname%%Loader::save(const QString & fileName, QObject * model) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (f.open(QIODevice::WriteOnly))
    {
        QDataStream ds(&f);

        //create indices
%%save_index%%
%%save_dbg_count%%
        // save each class separately
        ds << (quint32)1;
%%save_init%%
        ds << (%%Classname%%*)model;
%%save%%
        f.commit();

        // free up memory
        //Model::clearIndex();
%%save_clear%%
    }
}



#include "%%classname%%root.h"
#include <QDebug>
#include <QtQml>
#include <QQmlEngine>

#include "classes.h"

%%Classname%%Root::%%Classname%%Root(QObject *parent) :
    QObject(parent)
{
     QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    // singleton
    qmlRegisterType<%%Classname%%>();

    // object
    qmlRegisterType<ObjectList>();

    // all other classes that make up the model
    %%register_types%%
}

Undoer * %%Classname%%Root::undoer() const
{
    return Undoer::instance();
}

QObject * %%Classname%%Root::create()
{
    return new %%Classname%%;
}

QObject * %%Classname%%Root::load(const QString & fileName)
{
    %%clear_older%%

    %%Classname%% *ret = new %%Classname%%;

    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (f.open(QIODevice::ReadOnly))
    {
        quint32 count;
        QDataStream ds(&f);

        // all objects must be created before setting values and linking pointers
        %%load_init%%

        %%load_dbg_count%%

        // load
        ds >> ret;
        %%load%%
    }

    return ret;
}


void %%Classname%%Root::save(const QString & fileName,  QObject * model) const
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
        %%save_init%%

        ds << (%%Classname%%*)model;
        %%save%%


        f.commit();

        // free up memory
        //Model::clearIndex();
        %%save_clear%%
    }
}





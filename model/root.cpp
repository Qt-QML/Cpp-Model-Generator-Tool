#include "root.h"
#include <QDebug>
#include <QtQml>

#include "model/classes.h"
//#include "model_engine/objectlist.h"

Root::Root(QObject *parent) :
    QObject(parent)
{
    qmlRegisterType<ObjectList>();
    qmlRegisterType<ClassModel>();
    qmlRegisterType<ClassProp>();
    qmlRegisterType<Links>();
    qmlRegisterType<Undoer>();
    qmlRegisterType<Model>();
}

Undoer * Root::undoer() const
{
    return Undoer::instance();
}

Model * Root::model() const
{
    if( Model::_ptrs.count() == 0)
        new Model();

    return (Model::_ptrs.count() == 1 ? Model::_ptrs[0] : NULL);
}

void Root::open(const QString & fileName)
{
    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (f.open(QIODevice::ReadOnly))
    {
        quint32 count;
        QDataStream ds(&f);

        // all objects must be created before setting values and linking pointers
        ds >> count; Model::init(count);
        ds >> count; ClassModel::init(count);
        ds >> count; ClassProp::init(count);
        ds >> count; Links::init(count);

        qDebug() << "loadingodel: " << Model::_ptrs.count();
        qDebug() << "loading ClassModel: " << ClassModel::_ptrs.count();
        qDebug() << "loading ClassProp: " << ClassProp::_ptrs.count();
        qDebug() << "loading Links: " << Links::_ptrs.count();

        // load
        Model::load(ds);
        ClassModel::load(ds);
        ClassProp::load(ds);
        Links::load(ds);
    }
    else
        Model::init(1);


    emit modelChanged();
}



void Root::save(const QString & fileName) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (f.open(QIODevice::WriteOnly))
    {
        QDataStream ds(&f);

        //create indices
        Model::createIndex();
        ClassModel::createIndex();
        ClassProp::createIndex();
        Links::createIndex();
        qDebug() << "saving Model: " << Model::_ptrs.count();
        qDebug() << "saving ClassModel: " << ClassModel::_ptrs.count();
        qDebug() << "saving ClassProp: " << ClassProp::_ptrs.count();
        qDebug() << "saving Links: " << Links::_ptrs.count();

        // save each class separately
        ds << (quint32)Model::_ptrs.count();
        ds << (quint32)ClassModel::_ptrs.count();
        ds << (quint32)ClassProp::_ptrs.count();
        ds << (quint32)Links::_ptrs.count();

        Model::save(ds);
        ClassModel::save(ds);
        ClassProp::save(ds);
        Links::save(ds);

        f.commit();

        // free up memory
        Model::clearIndex();
        ClassModel::clearIndex();
        ClassProp::clearIndex();
        Links::clearIndex();
    }
}



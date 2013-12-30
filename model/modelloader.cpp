#include "modelloader.h"
#include "classes.h"
#include <QUrl>
#include <QSaveFile>
#include <QFile>
#include <QDebug>
#include <QQmlComponent>


ModelLoader::ModelLoader(QObject *parent) :
    QObject(parent)
{
    qmlRegisterType<ObjectList>();
    qmlRegisterType<ClassModel>();
    qmlRegisterType<ClassProp>();
    qmlRegisterType<Links>();
    qmlRegisterType<Undoer>();
    qmlRegisterType<Model>();
}


QObject * ModelLoader::create()
{
    return new Model;
}

QObject * ModelLoader::load(const QString & fileName)
{
    Model *ret = new Model;

    QUrl url(fileName);
    QFile f(url.toLocalFile());

    if (f.open(QIODevice::ReadOnly))
    {
        quint32 count;
        QDataStream ds(&f);

        // all objects must be created before setting values and linking pointers
        ds >> count;
        ds >> count; ClassModel::init(count);
        ds >> count; ClassProp::init(count);
        ds >> count; Links::init(count);

       // qDebug() << "loadingodel: " << Model::_ptrs.count();
        qDebug() << "loading ClassModel: " << ClassModel::_ptrs.count();
        qDebug() << "loading ClassProp: " << ClassProp::_ptrs.count();
        qDebug() << "loading Links: " << Links::_ptrs.count();

        // load
        ds >> ret;
        ClassModel::load(ds);
        ClassProp::load(ds);
        Links::load(ds);
    }

    return ret;
}

void ModelLoader::save(const QString & fileName, QObject * model) const
{
    QUrl url(fileName);

    QSaveFile f(url.toLocalFile());
    if (f.open(QIODevice::WriteOnly))
    {
        QDataStream ds(&f);

        //create indices
        //Model::createIndex();
        ClassModel::createIndex();
        ClassProp::createIndex();
        Links::createIndex();

        qDebug() << "saving ClassModel: " << ClassModel::_ptrs.count();
        qDebug() << "saving ClassProp: " << ClassProp::_ptrs.count();
        qDebug() << "saving Links: " << Links::_ptrs.count();

        // FIXME - tricky. Ask undoer, which lines it has removed in a list
        // save each class separately
        ds << (quint32)1;
        ds << (quint32)ClassModel::_ptrs.count();
        ds << (quint32)ClassProp::_ptrs.count();
        ds << (quint32)Links::_ptrs.count();

        ds << (Model*)model;
        ClassModel::save(ds);
        ClassProp::save(ds);
        Links::save(ds);

        f.commit();

        // free up memory
        //Model::clearIndex();
        ClassModel::clearIndex();
        ClassProp::clearIndex();
        Links::clearIndex();
    }
}



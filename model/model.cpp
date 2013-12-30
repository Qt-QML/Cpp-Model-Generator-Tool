#include "model.h"
#include <QSaveFile>
#include <QTextStream>
#include <QDebug>
#include <QUrl>
#include <QDir>

#include <QQmlComponent>

Model::Model(QObject *parent) :
    QObject(parent)
{
    _winRect = QRectF(100,100, 600, 400);;

    _classes = new ObjectList(ClassModel::staticMetaObject);
    _links = new ObjectList(Links::staticMetaObject);
}

Model::~Model()
{
    _classes->deleteLater();
    _links->deleteLater();
}


// ----[ links ] ----
ObjectList* Model::links() const
{
    return _links;
}
void Model::setLinks(ObjectList* val)
{
    if (val == _links)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setLinksImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "links",QVariant::fromValue(_links), QVariant::fromValue(val));
    cmd->setText("Set Links");
    Undoer::instance()->push(cmd);
}
void Model::setLinksImp(ObjectList* val)
{
    _links = val;
    emit linksChanged();
}

// ----[ classes ] ----
ObjectList* Model::classes() const
{
    return _classes;
}
void Model::setClasses(ObjectList* val)
{
    if (val == _classes)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setClassesImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "classes",QVariant::fromValue(_classes), QVariant::fromValue(val));
    cmd->setText("Set Classes");
    Undoer::instance()->push(cmd);
}
void Model::setClassesImp(ObjectList* val)
{
    _classes = val;
    emit classesChanged();
}


// ----[ name ] ----
QString Model::name() const
{
    return _name;
}
void Model::setName(QString val)
{
    if (val == _name)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setNameImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "name",QVariant(_name), QVariant(val));
    cmd->setText("Set RootClassName");
    Undoer::instance()->push(cmd);
}
void Model::setNameImp(QString val)
{
    qDebug() << val;
    _name = val;
    emit nameChanged();
}


// ----[ winRect ] ----
QRectF Model::winRect() const
{
    return _winRect;
}
void Model::setWinRect(QRectF val)
{
    if (val == _winRect)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setWinRectImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "winRect",QVariant(_winRect), QVariant(val));
    cmd->setText("Set WinRect");
    Undoer::instance()->push(cmd);
}
void Model::setWinRectImp(QRectF val)
{
    _winRect = val;
    emit winRectChanged();
}


QDataStream& operator<< (QDataStream& ds, const Model * p)
{
    // ----[ links SAVE ] ----
    ds << ((qint32)(p->_links->count()));
    for (int i=0; i<p->_links->count(); i++)
    {
        Links *o = p->_links->get<Links*>(i);
        ds << Links::_indexedPtrs.value(o);
    }

    // ----[ classes SAVE ] ----
    ds << ((qint32)(p->_classes->count()));
    for (int i=0; i<p->_classes->count(); i++)
    {
        ClassModel *o = p->_classes->get<ClassModel*>(i);
        ds << ClassModel::_indexedPtrs.value(o);
    }


    // ----[ winRect SAVE ] ----
    ds << p->_winRect;
    ds << p->_name;


    return ds;
}

QDataStream& operator>> (QDataStream& ds, Model * p)
{
    quint32 count;
    quint32 index;

    // ----[ links LOAD ] ----
    p->links()->removeAll();
    ds >> count;
    for (quint32 i=0; i<count; i++)
    {
        ds >> index;
        Links *obj  = (index == -1 ? NULL : Links::_ptrs[index]);
        p->links()->insertRow(obj,  p->links()->count());
    }

    // ----[ classes LOAD ] ----
    p->classes()->removeAll();
    ds >> count;
    for (quint32 i=0; i<count; i++)
    {
        ds >> index;
        ClassModel *obj  = (index == -1 ? NULL : ClassModel::_ptrs[index]);
        p->classes()->insertRow(obj,  p->classes()->count());
    }

    // ----[ winRect LOAD ] ----
    ds >> p->_winRect;
    ds >> p->_name;

    return ds;
}


void Model::generateFiles(const QString &folder) const
{
    QUrl url = QUrl(folder);

    if (QDir(url.toLocalFile()).exists())
    {
        ObjectList *classes = this->classes();

        qDebug() << "generating files into: folder" << url.toLocalFile();
        for (int i=0; i<classes->count(); i++)
        {
            ClassModel *classModel = classes->get<ClassModel*>(i);

            QSaveFile cpp(url.toLocalFile() + "/" + classModel->name().toLower() + ".cpp");
            cpp.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppDS(&cpp);
            cppDS << classModel->generateCPP(this).toLatin1().data();
            cpp.commit();

            QSaveFile h(url.toLocalFile() + "/" +  classModel->name().toLower() + ".h");
            h.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppH(&h);
            cppH << classModel->generateH().toLatin1().data();
            h.commit();
        }

        //save(url.toString() + "/project.cod");

        // now save classes.h
        {
            QString h;
            h = "#ifndef CLASSES"+_name.toUpper()+"_H\n"
                    "#define CLASSES"+_name.toUpper()+"_H\n\n"

                    "#include <QObject>\n"
                    "#include <QColor>\n"
                    "#include <QImage>\n"
                    "#include <QVariant>\n"
                    "#include <QPoint>\n"
                    "#include <QPointF>\n"
                    "#include <QList>\n"
                    "#include <QHash>\n"
                    "#include <QSet>\n"
                    "#include <QRectF>\n\n"
                    "class Undoer;\n"
                    "class "+_name+"Root;\n"
                    "class ObjectList;\n\n";

            QSet<QString> list;// first classes by this program, later classes that are external
            for (int i=0; i<classes->count(); i++)
            {
                ClassModel *classModel = classes->get<ClassModel*>(i);
                list.insert(classModel->name());

                for (int j=0; j<classModel->properties()->count(); j++)
                {
                    ClassProp *prop = classModel->properties()->get<ClassProp*>(j);

                    if (prop->type() == "QObject*")
                        if (prop->subType().length()>0)
                            list.insert(prop->subType());
                }
            }
            QSetIterator<QString> i(list);
            while (i.hasNext())
                h += QString("class ") + i.next() + ";\n";


            h += "\n#include \"../model_engine/objectlist.h\"\n"
                    "#include \"" + _name.toLower() + "root.h\"\n"
                    "#include \"../model_engine/undoer.h\"\n\n";

            for (int i=0; i<classes->count(); i++)
            {
                ClassModel *classModel = classes->get<ClassModel*>(i);
                h += QString("#include \"") + classModel->name().toLower() + ".h\"\n";
            }

            h += "\n#endif // CLASSES_H";
            QSaveFile hfile(url.toLocalFile() + "/classes.h");
            hfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream hDS(&hfile);
            hDS << h;
            hfile.commit();
        }

        {
            // now save root.cpp
            QFile tpl(":/resources/root.cpp");
            tpl.open(QIODevice::ReadOnly);
            QString cpp = tpl.readAll();
            tpl.close();

            QString tmp;


            QStringList names;
            for (int i=0; i<classes->count(); i++)
            {
                QString name = classes->get<ClassModel*>(i)->name(); // exclude root class
                if (name != _name)
                    names.append(name);
            }

            //qmlRegisterType<ObjectList>();
            //%%register_types%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("qmlRegisterType<%1>();\n").arg(names.at(i));
            cpp.replace("%%register_types%%", tmp, Qt::CaseSensitive);

            //ds >> count; Model::init(count);
            //%%load_init%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("ds >> count; %1::init(count);\n").arg(names.at(i));
            cpp.replace("%%load_init%%", tmp, Qt::CaseSensitive);

            //%%clear_older%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("%1::deleteAll();\n").arg(names.at(i));
            cpp.replace("%%clear_older%%", tmp, Qt::CaseSensitive);


            //qDebug() << "loadingodel: " << Model::_ptrs.count();
            //%%load_dbg_count%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("qDebug() << \"loading %1: \" << %2::_ptrs.count();\n").arg(names.at(i)).arg(names.at(i));
            cpp.replace("%%load_dbg_count%%", tmp, Qt::CaseSensitive);

            // ClassModel::load(ds);
            //%%load%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("%1::load(ds);\n").arg(names.at(i));
            cpp.replace("%%load%%", tmp, Qt::CaseSensitive);


            //Model::createIndex();
            //%%save_index%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("%1::createIndex();\n").arg(names.at(i));
            cpp.replace("%%save_index%%", tmp, Qt::CaseSensitive);


            //qDebug() << "saving Model: " << Model::_ptrs.count();
            //%%save_dbg_count%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("qDebug() << \"saving %1: \" << %2::_ptrs.count();\n").arg(names.at(i)).arg(names.at(i));
            cpp.replace("%%save_dbg_count%%", tmp, Qt::CaseSensitive);


            //ds << (quint32)Model::_ptrs.count();
            //%%save_init%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("ds << (quint32)%1::_ptrs.count();\n").arg(names.at(i));
            cpp.replace("%%save_init%%", tmp, Qt::CaseSensitive);


            //        Model::save(ds);
            //%%save%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("%1::save(ds);\n").arg(names.at(i));
            cpp.replace("%%save%%", tmp, Qt::CaseSensitive);


            //Model::clearIndex();
            //%%save_clear%%
            tmp = "";
            for (int i = 0; i < names.size(); ++i)
                tmp += QString("%1::clearIndex();\n").arg(names.at(i));
            cpp.replace("%%save_clear%%", tmp, Qt::CaseSensitive);


            cpp.replace("%%classname%%", _name.toLower(), Qt::CaseSensitive);
            cpp.replace("%%Classname%%", _name, Qt::CaseSensitive);


            QSaveFile cppfile(url.toLocalFile() + "/" + _name.toLower() + "root.cpp");
            cppfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppTS(&cppfile);
            cppTS << cpp;
            cppfile.commit();
        }



        {
            // now save root.cpp
            QFile tpl(":/resources/root.h");
            tpl.open(QIODevice::ReadOnly);
            QString cpp = tpl.readAll();
            tpl.close();

            cpp.replace("%%Classname%%", _name, Qt::CaseSensitive);
            cpp.replace("%%CLASSNAME%%", _name.toUpper(), Qt::CaseSensitive);

            QSaveFile cppfile(url.toLocalFile() + "/" + _name.toLower() + "root.h");
            cppfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppTS(&cppfile);
            cppTS << cpp;
            cppfile.commit();
        }
    }
}











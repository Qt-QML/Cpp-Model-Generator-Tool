#include "codegenerator.h"

#include "model/classmodel.h"
#include "model/classprop.h"

#include <QDebug>
#include <QDir>
#include <QSaveFile>
#include <QTextStream>
#include <QUrl>

static QString generateClassPropH(const ClassProp *classProp)
{
    QString h;

    h += "public:\n";

    h += "\tQ_PROPERTY(%%proptype%% %%propname%% ";
    if (classProp->read())
        h += " READ %%propname%% ";
    if (classProp->write())
        h += " WRITE set%%Propname%% ";

    if (classProp->notify())
        h += " NOTIFY %%propname%%Changed ";
    if (!classProp->write())
        h += " CONSTANT ";
    h += " )\n";

    if (classProp->validate().length() > 0)
        h += "\tbool validate%%Propname%%(%%proptype%% val) const;\n";

    if (classProp->read())
        h += "\t%%proptype%% %%propname%%() const;\n";

    if (classProp->write())
        h += "\tvoid set%%Propname%%(%%proptype%% val);\n";

    if (classProp->notify())
        h += "signals:\n\tvoid %%propname%%Changed();\n";

    h += "private:\n\tvoid set%%Propname%%Imp(%%proptype%% val);\n";

    h += "private:\n";
    h += "\t%%proptype%% _%%propname%%;\n";

    if (classProp->type() == "QObject*")
    {
        h += "private slots:\n"
                "\tvoid %%propname%%DeletedSlot();\n";
    }
    QString Name = classProp->name();
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = classProp->name();
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = classProp->type();
    if (type == "QObject*")
        type = classProp->subType() + "*";// pointer to subclass

    h.replace("%%proptype%%", type, Qt::CaseSensitive);
    h.replace("%%propname%%", name, Qt::CaseSensitive);
    h.replace("%%Propname%%", Name, Qt::CaseSensitive);

    return h;
}

static QString generateClassPropCPP(const ClassProp *classProp, const QString &classname)
{
    QString cpp;

    ////////////////////////////////////////////////////////////
    cpp += ""
            "%%proptype%% %%Classname%%::%%propname%%() const\n"
            "{\n"
            "\treturn _%%propname%%;\n"
            "}\n";


    ////////////////////////////////////////////////////////////
    if (classProp->write())
    {
        cpp +=  "void %%Classname%%::set%%Propname%%(%%proptype%% val)\n"
                "{\n";

        cpp += "\tif (val == _%%propname%%)\n"
                "\t\treturn;\n\n";

        if (classProp->undo())
        {
            cpp += "\tif (Undoer::instance()->noundo() == false)\n"
                    "\t{\n"
                    "\t\tset%%Propname%%Imp(val);\n"
                    "\t\treturn;\n"
                    "\t}\n\n";
        }

        if (classProp->validate().length() > 0)
            cpp += "\tif (!validate%%Propname%%(val))\n"
                    "\t\treturn;\n\n";

        if (classProp->undo())
        {
            cpp += "\tQUndoCommand *cmd = new PropertyChangeCmd(this, \"%%propname%%\",";

            if (!classProp->type().endsWith("*")) // if its not a pointer
                cpp += "QVariant(_%%propname%%), QVariant(val));\n";
            else
                cpp += "QVariant::fromValue(_%%propname%%), QVariant::fromValue(val));\n";

            cpp += "\tcmd->setText(\"Set %%Propname%%\");\n"
                    "\tUndoer::instance()->push(cmd);\n";
        }
        else if (classProp->notify()) // if no undo
        {
            cpp += "\t_%%propname%% = val;\n"
                    "\temit %%propname%%Changed();\n";
        }
        cpp += "}\n";
    }

    //implicit write
    ////////////////////////////////////////////////////////////////////////

    cpp += "void %%Classname%%::set%%Propname%%Imp(%%proptype%% val)\n"
            "{\n";
    if (classProp->type() == "QObject*")
    {
        cpp += "\tif (_%%propname%% != NULL)\n"
                "\t\tdisconnect(reinterpret_cast<QObject*>(_%%propname%%), SIGNAL(destroyed()), this, SLOT(%%propname%%DeletedSlot()));\n";
        cpp += "\tif (val != NULL)\n"
                "\t\tconnect(reinterpret_cast<QObject*>(val), SIGNAL(destroyed()), this, SLOT(%%propname%%DeletedSlot()));\n\n";
    }
    cpp += "\t_%%propname%% = val;\n";
    if (classProp->notify())
        cpp += "\temit %%propname%%Changed();\n";
    cpp +=  "}\n";

    ////////////////////////////////////////////////////////////////////////
    if (classProp->validate().length() > 0 && classProp->write())
    {
        cpp += "bool %%Classname%%::validate%%Propname%%(%%proptype%% val) const\n"
                "{\n"
                "\treturn %%validate%%;\n"
                "}\n";
    }
    ////////////////////////////////////////////////////////////////////////
    if (classProp->type() == "QObject*")
    {
        cpp += "void %%Classname%%::%%propname%%DeletedSlot()\n"
                "{\n"
                "\tset%%Propname%%(NULL);\n"
                "}\n";
    }

    ////////////////////////////////////////////////////////////////////////

    QString Name = classProp->name();
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = classProp->name();
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = classProp->type();
    if (type == "QObject*")
        type = classProp->subType() + "*";// pointer to subclass

    cpp.replace("%%validate%%", classProp->validate(), Qt::CaseSensitive);
    cpp.replace("%%Classname%%", classname, Qt::CaseSensitive);
    cpp.replace("%%propname%%", name, Qt::CaseSensitive);
    cpp.replace("%%Propname%%", Name, Qt::CaseSensitive);
    cpp.replace("%%proptype%%", type, Qt::CaseSensitive);

    return cpp;
}

static QString generateClassPropSave(const ClassProp *classProp)
{
    QString cpp;

    if (classProp->type() == "QObject*")
        cpp += "\tds << %%proptype%%::_indexedPtrs.value(p->_%%propname%%);";
    else if (classProp->type() == "ObjectList*")
    {
        cpp += "\tds << ((qint32)(p->_%%propname%%->count()));\n"
                "\tfor (int i=0; i<p->_%%propname%%->count(); i++)\n"
                "\t{\n"
                "\t\t%%proptype%% *o = p->_%%propname%%->get<%%proptype%%*>(i);\n"
                "\t\tds << %%proptype%%::_indexedPtrs.value(o);\n"
                "\t}\n";
    }
    else
        cpp += "\tds << p->_%%propname%%;\n";

    QString Name = classProp->name();
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = classProp->name();
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = classProp->type();
    if (type == "QObject*")
        type = classProp->subType(); // pointer to subclass
    else if (type == "ObjectList*")
        type = classProp->subType(); // pointer to subclass


    cpp.replace("%%propname%%", name, Qt::CaseSensitive);
    cpp.replace("%%Propname%%", Name, Qt::CaseSensitive);
    cpp.replace("%%proptype%%", type, Qt::CaseSensitive);


    return cpp;
}

static QString generateClassPropLoad(const ClassProp *classProp)
{
    QString cpp;

    if (classProp->type() == "QObject*")
        cpp += "\tds >> index; p->set%%Propname%%Imp((index == -1 ? NULL : %%proptype%%::_ptrs[index]));\n";
    else if (classProp->type() == "ObjectList*")
    {
        cpp += "\tp->%%propname%%()->removeAll();\n"
                "\tds >> count;\n"
                "\tfor (quint32 i=0; i<count; i++)\n"
                "\t{\n"
                "\t\tds >> index;\n"
                "\t\t%%proptype%% *obj  = (index == -1 ? NULL : %%proptype%%::_ptrs[index]);\n"
                "\t\tp->%%propname%%()->insertRow(obj,  p->%%propname%%()->count());\n"
                "\t}\n";
    }
    else
        cpp += "\tds >> p->_%%propname%%;\n";



    QString Name = classProp->name();
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = classProp->name();
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = classProp->type();
    if (type == "QObject*")
        type = classProp->subType(); // pointer to subclass
    else if (type == "ObjectList*")
        type = classProp->subType(); // pointer to subclass


    cpp.replace("%%propname%%", name, Qt::CaseSensitive);
    cpp.replace("%%Propname%%", Name, Qt::CaseSensitive);
    cpp.replace("%%proptype%%", type, Qt::CaseSensitive);


    return cpp;
}


static QString generateClassModelH(const ClassModel *classModel)
{
    QString cpp;

    cpp = "#ifndef %%CLASSNAME%%_H\n"
            "#define %%CLASSNAME%%_H\n\n"
            "#include \"classes.h\"\n\n"
            "class %%Classname%% : public QObject\n"
            "{\n"
            "\tQ_OBJECT\n"
            "public:\n"
            "\tQ_INVOKABLE %%Classname%%(QObject *parent = 0);\n"
            "\t~%%Classname%%();\n";

    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);
        cpp += QString("\n\n// ----[ ") + p->name() + " ] ----\n" + generateClassPropH(p) ;
    }

    cpp += "\n\npublic:\n"
            "\tfriend QDataStream& operator<< (QDataStream& ds, const %%Classname%% * p);\n"
            "\tfriend QDataStream& operator>> (QDataStream& ds, %%Classname%% * p);\n\n"
            "\tstatic void init(int count);\n"
            "\tstatic void load(QDataStream& ds);\n"
            "\tstatic void save(QDataStream& ds);\n"
            "\tstatic void createIndex();\n"
            "\tstatic void clearIndex();\n"
            "\tstatic void deleteAll();\n"
            "\tstatic QList<%%Classname%%*> _ptrs;\n"
            "\tstatic QHash<%%Classname%%*, quint32> _indexedPtrs;\n"
            "};\n"
            "QDataStream& operator<< (QDataStream& ds, const %%Classname%% * p);\n"
            "QDataStream& operator>> (QDataStream& ds, %%Classname%% * p);\n"
            "#endif // %%CLASSNAME%%_H\n";


    QString ClassName = classModel->name();
    QChar *ptr = ClassName.data();
    *ptr = ptr->toUpper();

    QString className = classModel->name();
    ptr = className.data();
    *ptr = ptr->toLower();

    QString CLASSNAME = classModel->name();
    CLASSNAME = CLASSNAME.toUpper();

    cpp.replace("%%classname%%", className, Qt::CaseSensitive);
    cpp.replace("%%Classname%%", ClassName, Qt::CaseSensitive);
    cpp.replace("%%CLASSNAME%%", CLASSNAME, Qt::CaseSensitive);

    return cpp;
}

static QString generateClassModelCPP(const ClassModel *classModel)
{
    QString cpp;

    cpp += "#include \"%%classname%%.h\"\n"
            "#include <QQmlEngine>\n\n";

    cpp += "//// - [ static ] ----------------------------------------------------------------------------\n"
            "QList<%%Classname%%*> %%Classname%%::_ptrs;\n"
            "QHash<%%Classname%% *, quint32> %%Classname%%::_indexedPtrs;\n\n"
            "void %%Classname%%::init(int count)\n"
            "{\n"
            "\t//clear current\n"
            "\tdeleteAll();\n"
            "\n"
            "\tfor (int i=0;i<count;i++)\n"
            "\t\tnew %%Classname%%;\n"
            "}\n\n"
            "void %%Classname%%::load(QDataStream& ds)\n"
            "{\n"
            "\tquint32 count = _ptrs.count();\n"
            "\tfor (quint32 i=0;i<count; i++)\n"
            "\t\tds >> _ptrs[i];\n"
            "}\n\n"
            "void %%Classname%%::save(QDataStream& ds)\n"
            "{\n"
            "\tQListIterator<%%Classname%% *> i(_ptrs);\n"
            "\twhile (i.hasNext())\n"
            "\t\tds << i.next();\n"
            "}\n\n"
            "void %%Classname%%::createIndex() //must be called for all classes before save for this class\n"
            "{\n"
            "\t_indexedPtrs.clear();\n"
            "\t_indexedPtrs.insert(NULL, -1);\n"
            "\tquint32 index = 0;\n"
            "\tQListIterator<%%Classname%% *> i(_ptrs);\n"
            "\twhile (i.hasNext())"
            "\t\t_indexedPtrs.insert(i.next(), index++);\n"
            "}\n\n"
            "void %%Classname%%::clearIndex()\n"
            "{\n"
            "\t_indexedPtrs.clear();\n"
            "}\n\n"
            "void %%Classname%%::deleteAll()\n"
            "{\n"
            "\tQList<%%Classname%%*> tmp = _ptrs;\n"
            "\t_ptrs.clear();\n"
            "\tfor (int i=0;i<tmp.count();i++)\n"
            "\t\tdelete tmp[i];\n"
            "}\n\n"
            "//// - [ non-static ] ----------------------------------------------------------------------------\n\n";

    cpp +=  "%%Classname%%::%%Classname%%(QObject *parent) :\n"
            "\tQObject(parent)\n"
            "{\n"
            "\t    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);\n";
    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);

        if (p->init().length() > 0)
            cpp += QString("\t") + p->init() + ";\n";
    }
    cpp += "\n\t_ptrs.append(this);\n";

    cpp += "}\n\n";

    cpp += "%%Classname%%::~%%Classname%%()\n"
            "{\n";
    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);

        if (p->destruct().length() > 0)
            cpp += QString("\t") + p->destruct() + ";\n";
    }
    cpp += "\n\t_ptrs.removeOne(this);\n";
    cpp += "}\n\n";

    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);
        cpp += QString("\n// ----[ ") + p->name() + " ] ----\n" + generateClassPropCPP(p, classModel->name());
    }





    cpp += "\n\nQDataStream& operator<< (QDataStream& ds, const %%Classname%% * p)\n"
            "{\n";
    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);
        if (p->save() == true)
            cpp += QString("\n\n// ----[ ") + p->name() + " SAVE ] ----\n" + generateClassPropSave(p);
    }
    cpp += "\n\n\treturn ds;\n"
            "}";

    cpp += "\n\nQDataStream& operator>> (QDataStream& ds, %%Classname%% * p)\n"
            "{\n"
            "\tquint32 count;\n"
            "\tquint32 index;\n";
    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);
        if (p->save() == true)
            cpp += QString("\n// ----[ ") + p->name() + " LOAD ] ----\n" + generateClassPropLoad(p);
    }
    cpp += "\n\n\treturn ds;\n"
            "}\n";



    QString ClassName = classModel->name();
    QChar *ptr = ClassName.data();
    *ptr = ptr->toUpper();

    QString className = classModel->name();
    ptr = className.data();
    *ptr = ptr->toLower();

    cpp.replace("%%classname%%", className, Qt::CaseSensitive);
    cpp.replace("%%Classname%%", ClassName, Qt::CaseSensitive);

    return cpp;
}


CodeGenerator::CodeGenerator(QObject *parent)
    : QObject(parent)
{
}

void CodeGenerator::generateFiles(QObject *modelObject, const QString &folder) const
{
    Model *model = qobject_cast<Model*>(modelObject);
    if (!model)
        return;

    QUrl url = QUrl(folder);

    if (QDir(url.toLocalFile()).exists())
    {
        ObjectList *classes = model->classes();

        qDebug() << "generating files into: folder" << url.toLocalFile();
        for (int i=0; i<classes->count(); i++)
        {
            ClassModel *classModel = classes->get<ClassModel*>(i);

            QSaveFile cpp(url.toLocalFile() + "/" + classModel->name().toLower() + ".cpp");
            cpp.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppDS(&cpp);
            cppDS << generateClassModelCPP(classModel).toLatin1().data();
            cpp.commit();

            QSaveFile h(url.toLocalFile() + "/" +  classModel->name().toLower() + ".h");
            h.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppH(&h);
            cppH << generateClassModelH(classModel).toLatin1().data();
            h.commit();
        }

        //save(url.toString() + "/project.cod");

        // now save classes.h
        {
            QString h;
            h = "#ifndef CLASSES"+model->name().toUpper()+"_H\n"
                    "#define CLASSES"+model->name().toUpper()+"_H\n\n"

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
                    "class "+model->name()+"Root;\n"
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
                    "#include \"" + model->name().toLower() + "root.h\"\n"
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
                if (name != model->name())
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


            cpp.replace("%%classname%%", model->name().toLower(), Qt::CaseSensitive);
            cpp.replace("%%Classname%%", model->name(), Qt::CaseSensitive);


            QSaveFile cppfile(url.toLocalFile() + "/" + model->name().toLower() + "root.cpp");
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

            cpp.replace("%%Classname%%", model->name(), Qt::CaseSensitive);
            cpp.replace("%%CLASSNAME%%", model->name().toUpper(), Qt::CaseSensitive);

            QSaveFile cppfile(url.toLocalFile() + "/" + model->name().toLower() + "root.h");
            cppfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppTS(&cppfile);
            cppTS << cpp;
            cppfile.commit();
        }
    }
}

#include "codegenerator.h"

#include "model/classmodel.h"
#include "model/classprop.h"

#include <QDebug>
#include <QDir>
#include <QSaveFile>
#include <QTextStream>
#include <QUrl>

static void replacePropertyNames(QString &string, const ClassProp *classProp)
{
    QString Name = classProp->name();
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = classProp->name();
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = classProp->type();
    if (type == "QObject*")
        type = classProp->subType() + "*";// pointer to subclass

    string.replace("%%proptype%%", type);
    string.replace("%%subtype%%", classProp->subType());
    string.replace("%%propname%%", name);
    string.replace("%%Propname%%", Name);
}

static QString generateClassPropH(const ClassProp *classProp)
{
    QString h;

    h += "public:\n";

    h += "    Q_PROPERTY(%%proptype%% %%propname%% ";
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
        h += "    bool validate%%Propname%%(%%proptype%% val) const;\n";

    if (classProp->read())
        h += "    %%proptype%% %%propname%%() const;\n";

    if (classProp->write())
        h += "    void set%%Propname%%(%%proptype%% val);\n";

    if (classProp->notify())
        h += "signals:\n    void %%propname%%Changed();\n";

    h += "private:\n    void set%%Propname%%Imp(%%proptype%% val);\n";

    h += "private:\n";
    h += "    %%proptype%% _%%propname%%;\n";

    if (classProp->type() == "QObject*")
    {
        h += "private slots:\n"
             "    void %%propname%%DeletedSlot();\n";
    }

    replacePropertyNames(h, classProp);

    return h;
}

static QString generateClassPropCPP(const ClassProp *classProp, const QString &classname)
{
    QString cpp;

    ////////////////////////////////////////////////////////////
    cpp += "%%proptype%% %%Classname%%::%%propname%%() const\n"
           "{\n"
           "    return _%%propname%%;\n"
           "}\n";


    ////////////////////////////////////////////////////////////
    if (classProp->write())
    {
        cpp += "void %%Classname%%::set%%Propname%%(%%proptype%% val)\n"
               "{\n";

        cpp += "    if (val == _%%propname%%)\n"
               "        return;\n\n";

        if (classProp->undo())
        {
            cpp += "    if (Undoer::instance()->noundo() == false)\n"
                   "    {\n"
                   "        set%%Propname%%Imp(val);\n"
                   "        return;\n"
                   "    }\n\n";
        }

        if (classProp->validate().length() > 0)
            cpp += "    if (!validate%%Propname%%(val))\n"
                   "        return;\n\n";

        if (classProp->undo())
        {
            cpp += "    QUndoCommand *cmd = new PropertyChangeCmd(this, \"%%propname%%\",";

            if (!classProp->type().endsWith("*")) // if its not a pointer
                cpp += "QVariant(_%%propname%%), QVariant(val));\n";
            else
                cpp += "QVariant::fromValue(_%%propname%%), QVariant::fromValue(val));\n";

            cpp += "    cmd->setText(\"Set %%Propname%%\");\n"
                   "    Undoer::instance()->push(cmd);\n";
        }
        else if (classProp->notify()) // if no undo
        {
            cpp += "    _%%propname%% = val;\n"
                   "    emit %%propname%%Changed();\n";
        }
        cpp += "}\n";
    }

    //implicit write
    ////////////////////////////////////////////////////////////////////////

    cpp += "void %%Classname%%::set%%Propname%%Imp(%%proptype%% val)\n"
            "{\n";
    if (classProp->type() == "QObject*")
    {
        cpp += "    if (_%%propname%%)\n"
                "        disconnect(_%%propname%%, SIGNAL(destroyed()), this, SLOT(%%propname%%DeletedSlot()));\n";
        cpp += "    if (val)\n"
                "        connect(val, SIGNAL(destroyed()), this, SLOT(%%propname%%DeletedSlot()));\n\n";
    }
    cpp += "    _%%propname%% = val;\n";
    if (classProp->notify())
        cpp += "    emit %%propname%%Changed();\n";
    cpp +=  "}\n";

    ////////////////////////////////////////////////////////////////////////
    if (classProp->validate().length() > 0 && classProp->write())
    {
        cpp += "bool %%Classname%%::validate%%Propname%%(%%proptype%% val) const\n"
                "{\n"
                "    return %%validate%%;\n"
                "}\n";
    }
    ////////////////////////////////////////////////////////////////////////
    if (classProp->type() == "QObject*")
    {
        cpp += "void %%Classname%%::%%propname%%DeletedSlot()\n"
                "{\n"
                "    set%%Propname%%(Q_NULLPTR);\n"
                "}\n";
    }

    ////////////////////////////////////////////////////////////////////////

    replacePropertyNames(cpp, classProp);

    cpp.replace("%%validate%%", classProp->validate());
    cpp.replace("%%Classname%%", classname);

    return cpp;
}

static QString generateClassPropSave(const ClassProp *classProp)
{
    QString cpp;

    if (classProp->type() == "QObject*")
    {
        cpp += "    ds << %%subtype%%::_indexedPtrs.value(p->_%%propname%%);";
    }
    else if (classProp->type() == "ObjectList*")
    {
        cpp += "    ds << ((qint32)(p->_%%propname%%->count()));\n"
               "    for (int i=0; i<p->_%%propname%%->count(); i++)\n"
               "    {\n"
               "        %%subtype%% *o = p->_%%propname%%->get<%%subtype%%*>(i);\n"
               "        ds << %%subtype%%::_indexedPtrs.value(o);\n"
               "    }\n";
    }
    else
    {
        cpp += "    ds << p->_%%propname%%;\n";
    }

    replacePropertyNames(cpp, classProp);

    return cpp;
}

static QString generateClassPropLoad(const ClassProp *classProp)
{
    QString cpp;

    if (classProp->type() == "QObject*")
    {
        cpp += "    ds >> index; p->set%%Propname%%Imp((index == -1 ? Q_NULLPTR : %%subtype%%::_ptrs[index]));\n";
    }
    else if (classProp->type() == "ObjectList*")
    {
        cpp += "    p->%%propname%%()->removeAll();\n"
               "    ds >> count;\n"
               "    for (quint32 i=0; i<count; i++)\n"
               "    {\n"
               "        ds >> index;\n"
               "        %%subtype%% *obj = (index == -1 ? Q_NULLPTR : %%subtype%%::_ptrs[index]);\n"
               "        p->%%propname%%()->insertRow(obj,  p->%%propname%%()->count());\n"
               "    }\n";
    }
    else
    {
        cpp += "    ds >> p->_%%propname%%;\n";
    }

    replacePropertyNames(cpp, classProp);

    return cpp;
}

static QString generateClassPropToJson(const ClassProp *classProp)
{
    QString cpp;

    if (classProp->type() == "QObject*")
    {
        cpp += "    object.insert(QLatin1String(\"%%propname%%\"),\n"
               "                  %%subtype%%::_indexedPtrs.value(p->_%%propname%%));\n";
    }
    else if (classProp->type() == "ObjectList*")
    {
        cpp += "    {\n"
               "        QJsonArray array;\n"
               "        for (QObject *o : p->_%%propname%%->list())\n"
               "            array.append(%%subtype%%::_indexedPtrs.value(static_cast<%%subtype%%*>(o)));\n"
               "        object.insert(QLatin1String(\"%%propname%%\"), array);\n"
               "    }\n";
    }
    else
    {
        cpp += "    object.insert(QLatin1String(\"%%propname%%\"), toJson(p->_%%propname%%));\n";
    }

    replacePropertyNames(cpp, classProp);

    return cpp;
}

static QString generateClassPropFromJson(const ClassProp *classProp)
{
    QString cpp;

    if (classProp->type() == "QObject*")
    {
        cpp += "    {\n"
               "        int index = object.value(QLatin1String(\"%%propname%%\")).toInt(-1);\n"
               "        p->set%%Propname%%Imp(index == -1 ? Q_NULLPTR : %%subtype%%::_ptrs[index]);\n"
               "    }\n";
    }
    else if (classProp->type() == "ObjectList*")
    {
        cpp += "    p->_%%propname%%->removeAll();\n"
               "    for (const QJsonValue &value : object.value(\"%%propname%%\").toArray()) {\n"
               "        int index = value.toInt(-1);\n"
               "        %%subtype%% *obj = (index == -1 ? Q_NULLPTR : %%subtype%%::_ptrs[index]);\n"
               "        p->%%propname%%()->insertRow(obj, p->_%%propname%%->count());\n"
               "    }\n";
    }
    else
    {
        cpp += "    fromJson(object.value(QLatin1String(\"%%propname%%\")), p->_%%propname%%);\n";
    }

    replacePropertyNames(cpp, classProp);

    return cpp;
}

static QString generateClassModelH(const ClassModel *classModel, bool isRoot)
{
    QString cpp;

    cpp = "#ifndef %%CLASSNAME%%_H\n"
          "#define %%CLASSNAME%%_H\n"
          "\n"
          "#include \"classes.h\"\n"
          "\n"
          "class %%Classname%% : public QObject\n"
          "{\n"
          "    Q_OBJECT\n"
          "public:\n"
          "    Q_INVOKABLE %%Classname%%(QObject *parent = Q_NULLPTR);\n"
          "    ~%%Classname%%();\n";

    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);
        cpp += QString("\n\n    // ----[ ") + p->name() + " ] ----\n" + generateClassPropH(p) ;
    }

    cpp += "\npublic:\n"
            "    friend QDataStream& operator<< (QDataStream& ds, const %%Classname%% * p);\n"
            "    friend QDataStream& operator>> (QDataStream& ds, %%Classname%% * p);\n"
            "\n"
            "    friend QJsonObject toJson(const %%Classname%% *p);\n"
            "    friend void fromJson(const QJsonValue &value, %%Classname%% *p);\n"
            "\n";

    if (!isRoot) {
        cpp += "    static void init(int count);\n"
               "    static void load(QDataStream& ds);\n"
               "    static void save(QDataStream& ds);\n"
               "    static void loadFromJson(const QJsonArray &);\n"
               "    static QJsonArray saveToJson();\n"
               "    static void createIndex();\n"
               "    static void clearIndex();\n"
               "    static void deleteAll();\n"
               "    static QList<%%Classname%%*> _ptrs;\n"
               "    static QHash<%%Classname%%*, int> _indexedPtrs;\n";
    }

    cpp += "};\n"
           "QDataStream& operator<< (QDataStream& ds, const %%Classname%% * p);\n"
           "QDataStream& operator>> (QDataStream& ds, %%Classname%% * p);\n"
            "\n"
            "QJsonObject toJson(const %%Classname%% *p);\n"
            "void fromJson(const QJsonValue &value, %%Classname%% *p);\n"
            "\n"
            "#endif // %%CLASSNAME%%_H\n";

    QString ClassName = classModel->name();
    QChar *ptr = ClassName.data();
    *ptr = ptr->toUpper();

    QString className = classModel->name();
    ptr = className.data();
    *ptr = ptr->toLower();

    QString CLASSNAME = classModel->name();
    CLASSNAME = CLASSNAME.toUpper();

    cpp.replace("%%classname%%", className);
    cpp.replace("%%Classname%%", ClassName);
    cpp.replace("%%CLASSNAME%%", CLASSNAME);

    return cpp;
}

static QString generateClassModelCPP(const ClassModel *classModel, bool isRoot)
{
    QString cpp;

    cpp += "#include \"%%classname%%.h\"\n"
            "#include <QQmlEngine>\n\n";

    if (!isRoot) {
        cpp += "//// - [ static ] ----------------------------------------------------------------------------\n"
               "QList<%%Classname%%*> %%Classname%%::_ptrs;\n"
               "QHash<%%Classname%% *, int> %%Classname%%::_indexedPtrs;\n\n"
               "void %%Classname%%::init(int count)\n"
               "{\n"
               "    //clear current\n"
               "    deleteAll();\n"
               "\n"
               "    for (int i=0;i<count;i++)\n"
               "        new %%Classname%%;\n"
               "}\n\n"
               "void %%Classname%%::load(QDataStream& ds)\n"
               "{\n"
               "    quint32 count = _ptrs.count();\n"
               "    for (quint32 i=0;i<count; i++)\n"
               "        ds >> _ptrs[i];\n"
               "}\n\n"
               "void %%Classname%%::save(QDataStream& ds)\n"
               "{\n"
               "    QListIterator<%%Classname%% *> i(_ptrs);\n"
               "    while (i.hasNext())\n"
               "        ds << i.next();\n"
               "}\n\n"
               "void %%Classname%%::loadFromJson(const QJsonArray &array)\n"
               "{\n"
               "    for (int i = 0; i < _ptrs.size(); ++i)\n"
               "        fromJson(array.at(i), _ptrs[i]);\n"
               "}\n"
               "\n"
               "QJsonArray %%Classname%%::saveToJson()\n"
               "{\n"
               "    QJsonArray array;\n"
               "    for (auto p : _ptrs)\n"
               "        array.append(toJson(p));\n"
               "    return array;\n"
               "}\n"
               "\n"
               "void %%Classname%%::createIndex() //must be called for all classes before save for this class\n"
               "{\n"
               "    _indexedPtrs.clear();\n"
               "    _indexedPtrs.insert(NULL, -1);\n"
               "    quint32 index = 0;\n"
               "    QListIterator<%%Classname%% *> i(_ptrs);\n"
               "    while (i.hasNext())"
               "        _indexedPtrs.insert(i.next(), index++);\n"
               "}\n\n"
               "void %%Classname%%::clearIndex()\n"
               "{\n"
               "    _indexedPtrs.clear();\n"
               "}\n\n"
               "void %%Classname%%::deleteAll()\n"
               "{\n"
               "    QList<%%Classname%%*> tmp = _ptrs;\n"
               "    _ptrs.clear();\n"
               "    for (int i=0;i<tmp.count();i++)\n"
               "        delete tmp[i];\n"
               "}\n\n"
               "//// - [ non-static ] ----------------------------------------------------------------------------\n\n";
    }

    cpp += "%%Classname%%::%%Classname%%(QObject *parent) :\n"
           "    QObject(parent)\n"
           "{\n";

    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);

        if (p->init().length() > 0)
            cpp += QString("    ") + p->init() + ";\n";
    }

    if (!isRoot)
        cpp += "\n    _ptrs.append(this);\n";

    cpp += "}\n\n";

    cpp += "%%Classname%%::~%%Classname%%()\n"
            "{\n";
    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);

        if (p->destruct().length() > 0)
            cpp += QString("    ") + p->destruct() + ";\n";
    }

    if (!isRoot)
        cpp += "\n    _ptrs.removeOne(this);\n";
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
            cpp += QString("\n\n    // ----[ ") + p->name() + " SAVE ] ----\n" + generateClassPropSave(p);
    }
    cpp += "\n\n    return ds;\n"
            "}";

    cpp += "\n\nQDataStream& operator>> (QDataStream& ds, %%Classname%% * p)\n"
            "{\n"
            "    quint32 count;\n"
            "    quint32 index;\n";
    for (int i=0;i<classModel->properties()->count(); i++)
    {
        ClassProp *p =  classModel->properties()->get<ClassProp*>(i);
        if (p->save())
            cpp += QString("\n    // ----[ ") + p->name() + " LOAD ] ----\n" + generateClassPropLoad(p);
    }
    cpp += "\n\n    return ds;\n"
            "}\n";

    cpp += "\n\nQJsonObject toJson(const %%Classname%% *p)\n"
           "{\n"
           "    QJsonObject object;\n";

    for (QObject *obj : classModel->properties()->list()) {
        ClassProp *p = static_cast<ClassProp*>(obj);
        if (p->save())
            cpp += generateClassPropToJson(p);
    }

    cpp += "    return object;\n"
           "}\n\n";

    cpp += "void fromJson(const QJsonValue &value, %%Classname%% *p)\n"
           "{\n"
           "    const QJsonObject object = value.toObject();\n";

    for (QObject *obj : classModel->properties()->list()) {
        ClassProp *p = static_cast<ClassProp*>(obj);
        if (p->save())
            cpp += generateClassPropFromJson(p);
    }

    cpp += "}\n";

    QString ClassName = classModel->name();
    QChar *ptr = ClassName.data();
    *ptr = ptr->toUpper();

    QString classname = classModel->name().toLower();

    cpp.replace("%%classname%%", classname);
    cpp.replace("%%Classname%%", ClassName);

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
            bool isRoot = model->name() == classModel->name();

            QSaveFile cpp(url.toLocalFile() + "/" + classModel->name().toLower() + ".cpp");
            cpp.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppDS(&cpp);
            cppDS << generateClassModelCPP(classModel, isRoot).toLatin1().data();
            cpp.commit();

            QSaveFile h(url.toLocalFile() + "/" +  classModel->name().toLower() + ".h");
            h.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppH(&h);
            cppH << generateClassModelH(classModel, isRoot).toLatin1().data();
            h.commit();
        }

        //save(url.toString() + "/project.cod");

        // now save classes.h
        {
            QString h;
            h = "#ifndef CLASSES" + model->name().toUpper() + "_H\n"
                "#define CLASSES" + model->name().toUpper() + "_H\n"
                "\n"
                "#include <QObject>\n"
                "#include <QColor>\n"
                "#include <QImage>\n"
                "#include <QVariant>\n"
                "#include <QPoint>\n"
                "#include <QPointF>\n"
                "#include <QList>\n"
                "#include <QHash>\n"
                "#include <QSet>\n"
                "#include <QRectF>\n"
                "\n";

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

            h += "\n#include \"../model_engine/jsonconverters.h\"\n"
                 "#include \"../model_engine/objectlist.h\"\n"
                 "#include \"../model_engine/undoer.h\"\n\n";

            for (int i=0; i<classes->count(); i++)
            {
                ClassModel *classModel = classes->get<ClassModel*>(i);
                h += QString("#include \"") + classModel->name().toLower() + ".h\"\n";
            }

            h += "\n#endif // CLASSES"+model->name().toUpper()+"_H\n";
            QSaveFile hfile(url.toLocalFile() + "/classes.h");
            hfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream hDS(&hfile);
            hDS << h;
            hfile.commit();
        }

        {
            // now save [root]loader.cpp
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
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    qmlRegisterType<%1>();\n").arg(name);
            cpp.replace("%%register_types%%", tmp);

            //ds >> count; Model::init(count);
            //%%load_init%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    ds >> count; %1::init(count);\n").arg(name);
            cpp.replace("%%load_init%%", tmp);

            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    %1::init(rootObject[\"%1\"].toArray().size());\n").arg(name);
            cpp.replace("%%load_init_json%%", tmp);

            //qDebug() << "loadingodel: " << Model::_ptrs.count();
            //%%load_dbg_count%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    qDebug() << \"loading %1: \" << %1::_ptrs.count();\n").arg(name);
            cpp.replace("%%load_dbg_count%%", tmp);

            // ClassModel::load(ds);
            //%%load%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    %1::load(ds);\n").arg(name);
            cpp.replace("%%load%%", tmp);

            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    %1::loadFromJson(rootObject[QLatin1String(\"%1\")].toArray());\n").arg(name);
            cpp.replace("%%load_json%%", tmp);

            //Model::createIndex();
            //%%save_index%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    %1::createIndex();\n").arg(name);
            cpp.replace("%%save_index%%", tmp);


            //qDebug() << "saving Model: " << Model::_ptrs.count();
            //%%save_dbg_count%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    qDebug() << \"saving %1: \" << %1::_ptrs.count();\n").arg(name);
            cpp.replace("%%save_dbg_count%%", tmp);


            //ds << (quint32)Model::_ptrs.count();
            //%%save_init%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    ds << (quint32)%1::_ptrs.count();\n").arg(name);
            cpp.replace("%%save_init%%", tmp);


            //        Model::save(ds);
            //%%save%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    %1::save(ds);\n").arg(name);
            cpp.replace("%%save%%", tmp);

            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    rootObject.insert(QLatin1String(\"%1\"), %1::saveToJson());\n").arg(name);
            cpp.replace("%%save_json%%", tmp);



            //Model::clearIndex();
            //%%save_clear%%
            tmp.clear();
            for (const QString &name : names)
                tmp += QString("    %1::clearIndex();\n").arg(name);
            cpp.replace("%%save_clear%%", tmp);


            cpp.replace("%%classname%%", model->name().toLower());
            cpp.replace("%%Classname%%", model->name());


            QSaveFile cppfile(url.toLocalFile() + "/" + model->name().toLower() + "loader.cpp");
            cppfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppTS(&cppfile);
            cppTS << cpp;
            cppfile.commit();
        }



        {
            // now save [root]loader.h
            QFile tpl(":/resources/root.h");
            tpl.open(QIODevice::ReadOnly);
            QString cpp = tpl.readAll();
            tpl.close();

            cpp.replace("%%Classname%%", model->name());
            cpp.replace("%%CLASSNAME%%", model->name().toUpper());

            QSaveFile cppfile(url.toLocalFile() + "/" + model->name().toLower() + "loader.h");
            cppfile.open(QIODevice::WriteOnly | QIODevice::Truncate);
            QTextStream cppTS(&cppfile);
            cppTS << cpp;
            cppfile.commit();
        }
    }
}

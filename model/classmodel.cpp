#include "classModel.h"
#include <QDebug>

//// - [ static ] ----------------------------------------------------------------------------
QList<ClassModel*> ClassModel::_ptrs;
QHash<ClassModel *, quint32> ClassModel::_indexedPtrs;

void ClassModel::init(int count)
{
    //clear current
    while (!_ptrs.isEmpty())
        _ptrs.takeFirst()->deleteLater();
    _ptrs.clear();
    for (int i=0;i<count;i++)
        new ClassModel;
}

void ClassModel::load(QDataStream& ds)
{
    quint32 count = _ptrs.count();
    for (quint32 i=0;i<count; i++)
        ds >> _ptrs[i];
}

void ClassModel::save(QDataStream& ds)
{
    QListIterator<ClassModel *> i(_ptrs);
    while (i.hasNext())
        ds << i.next();
}

void ClassModel::createIndex() //must be called for all classes before save for this class
{
    _indexedPtrs.clear();
    _indexedPtrs.insert(NULL, -1);
    quint32 index = 0;
    QListIterator<ClassModel *> i(_ptrs);
    while (i.hasNext())		_indexedPtrs.insert(i.next(), index++);
}

void ClassModel::clearIndex()
{
    _indexedPtrs.clear();
}

//// - [ non-static ] ----------------------------------------------------------------------------

ClassModel::ClassModel(QObject *parent) :
    QObject(parent)
{
    _properties = new ObjectList(ClassProp::staticMetaObject);

    _ptrs.append(this);
}

ClassModel::~ClassModel()
{
    _properties->deleteLater();

    _ptrs.removeOne(this);
}


// ----[ properties ] ----
ObjectList* ClassModel::properties() const
{
    return _properties;
}
void ClassModel::setProperties(ObjectList* val)
{
    if (val == _properties)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setPropertiesImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "properties",QVariant::fromValue(_properties), QVariant::fromValue(val));
    cmd->setText("Set Properties");
    Undoer::instance()->push(cmd);
}
void ClassModel::setPropertiesImp(ObjectList* val)
{
    _properties = val;
    emit propertiesChanged();
}

// ----[ name ] ----
QString ClassModel::name() const
{
    return _name;
}
void ClassModel::setName(QString val)
{
    if (val == _name)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setNameImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "name",QVariant(_name), QVariant(val));
    cmd->setText("Set Name");
    Undoer::instance()->push(cmd);
}
void ClassModel::setNameImp(QString val)
{
    _name = val;
    emit nameChanged();
}

// ----[ pos ] ----
QPointF ClassModel::pos() const
{
    return _pos;
}
void ClassModel::setPos(QPointF val)
{
    if (val == _pos)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setPosImp(val);
        return;
    }
    QUndoCommand *cmd = new PropertyChangeCmd(this, "pos",QVariant(_pos), QVariant(val));
    cmd->setText("Set Pos");
    Undoer::instance()->push(cmd);
}
void ClassModel::setPosImp(QPointF val)
{
    _pos = val;
    qDebug() << _pos;
    emit posChanged();
}

// ----[ subtype ] ----
QPointF ClassModel::cpos() const
{
    return _cpos;
}
void ClassModel::setCPos(QPointF val)
{
    if (val == _cpos)
        return;

    setCPosImp(val);

}
void ClassModel::setCPosImp(QPointF val)
{
    _cpos = val;
    emit cposChanged();
}


QDataStream& operator<< (QDataStream& ds, const ClassModel * p)
{


    // ----[ properties SAVE ] ----
    ds << ((qint32)(p->_properties->count()));
    for (int i=0; i<p->_properties->count(); i++)
    {
        ClassProp *o = p->_properties->get<ClassProp*>(i);
        ds << ClassProp::_indexedPtrs.value(o);
    }


    // ----[ name SAVE ] ----
    ds << p->_name;


    // ----[ pos SAVE ] ----
    ds << p->_pos;


    return ds;
}

QDataStream& operator>> (QDataStream& ds, ClassModel * p)
{
    quint32 count;
    quint32 index;

    // ----[ properties LOAD ] ----
    p->properties()->removeAll();
    ds >> count;
    for (quint32 i=0; i<count; i++)
    {
        ds >> index;
        ClassProp *obj  = (index == -1 ? NULL : ClassProp::_ptrs[index]);
        p->properties()->insertRow(obj,  p->properties()->count());
    }

    // ----[ name LOAD ] ----
    ds >> p->_name;

    // ----[ pos LOAD ] ----
    ds >> p->_pos;


    return ds;
}













QString ClassModel::generateH() const
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

    for (int i=0;i<_properties->count(); i++)
    {
        ClassProp *p =  _properties->get<ClassProp*>(i);
        cpp += QString("\n\n// ----[ ") + p->name() + " ] ----\n" + p->generateH() ;
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


    QString ClassName = _name;
    QChar *ptr = ClassName.data();
    *ptr = ptr->toUpper();

    QString className = _name;
    ptr = className.data();
    *ptr = ptr->toLower();

    QString CLASSNAME = _name;
    CLASSNAME = CLASSNAME.toUpper();

    cpp.replace("%%classname%%", className, Qt::CaseSensitive);
    cpp.replace("%%Classname%%", ClassName, Qt::CaseSensitive);
    cpp.replace("%%CLASSNAME%%", CLASSNAME, Qt::CaseSensitive);

    return cpp;
}

QString ClassModel::generateCPP(const Model *model) const
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
    for (int i=0;i<_properties->count(); i++)
    {
        ClassProp *p =  _properties->get<ClassProp*>(i);

        if (p->init().length() > 0)
            cpp += QString("\t") + p->init() + ";\n";
    }
    cpp += "\n\t_ptrs.append(this);\n";

    cpp += "}\n\n";

    cpp += "%%Classname%%::~%%Classname%%()\n"
            "{\n";
    for (int i=0;i<_properties->count(); i++)
    {
        ClassProp *p =  _properties->get<ClassProp*>(i);

        if (p->destruct().length() > 0)
            cpp += QString("\t") + p->destruct() + ";\n";
    }
    cpp += "\n\t_ptrs.removeOne(this);\n";
    cpp += "}\n\n";

    for (int i=0;i<_properties->count(); i++)
    {
        ClassProp *p =  _properties->get<ClassProp*>(i);
        cpp += QString("\n// ----[ ") + p->name() + " ] ----\n" + p->generateCPP(_name) ;
    }





    cpp += "\n\nQDataStream& operator<< (QDataStream& ds, const %%Classname%% * p)\n"
            "{\n";
    for (int i=0;i<_properties->count(); i++)
    {
        ClassProp *p =  _properties->get<ClassProp*>(i);
        if (p->save() == true)
            cpp += QString("\n\n// ----[ ") + p->name() + " SAVE ] ----\n" + p->generateSave();
    }
    cpp += "\n\n\treturn ds;\n"
            "}";

    cpp += "\n\nQDataStream& operator>> (QDataStream& ds, %%Classname%% * p)\n"
            "{\n"
            "\tquint32 count;\n"
            "\tquint32 index;\n";
    for (int i=0;i<_properties->count(); i++)
    {
        ClassProp *p =  _properties->get<ClassProp*>(i);
        if (p->save() == true)
            cpp += QString("\n// ----[ ") + p->name() + " LOAD ] ----\n" + p->generateLoad();
    }
    cpp += "\n\n\treturn ds;\n"
            "}\n";



    QString ClassName = _name;
    QChar *ptr = ClassName.data();
    *ptr = ptr->toUpper();

    QString className = _name;
    ptr = className.data();
    *ptr = ptr->toLower();

    cpp.replace("%%classname%%", className, Qt::CaseSensitive);
    cpp.replace("%%Classname%%", ClassName, Qt::CaseSensitive);

    return cpp;
}


#include "classProp.h"
//// - [ static ] ----------------------------------------------------------------------------
QList<ClassProp*> ClassProp::_ptrs;
QHash<ClassProp *, quint32> ClassProp::_indexedPtrs;

void ClassProp::init(int count)
{
    //clear current
    while (!_ptrs.isEmpty())
        _ptrs.takeFirst()->deleteLater();
    _ptrs.clear();
    for (int i=0;i<count;i++)
        new ClassProp;
}

void ClassProp::load(QDataStream& ds)
{
    quint32 count = _ptrs.count();
    for (quint32 i=0;i<count; i++)
        ds >> _ptrs[i];
}

void ClassProp::save(QDataStream& ds)
{
    QListIterator<ClassProp *> i(_ptrs);
    while (i.hasNext())
        ds << i.next();
}

void ClassProp::createIndex() //must be called for all classes before save for this class
{
    _indexedPtrs.clear();
    _indexedPtrs.insert(NULL, -1);
    quint32 index = 0;
    QListIterator<ClassProp *> i(_ptrs);
    while (i.hasNext())		_indexedPtrs.insert(i.next(), index++);
}

void ClassProp::clearIndex()
{
    _indexedPtrs.clear();
}

//// - [ non-static ] ----------------------------------------------------------------------------

ClassProp::ClassProp(QObject *parent) :
    QObject(parent)
{
    _count = 0;
    _ptrs.append(this);
}

ClassProp::~ClassProp()
{

    _ptrs.removeOne(this);
}


// ----[ read ] ----
bool ClassProp::read() const
{
    return _read;
}
void ClassProp::setRead(bool val)
{
    if (val == _read)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setReadImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "read",QVariant(_read), QVariant(val));
    cmd->setText("Set Read");
    Undoer::instance()->push(cmd);
}
void ClassProp::setReadImp(bool val)
{
    _read = val;
    emit readChanged();
}

// ----[ save ] ----
bool ClassProp::save() const
{
    return _save;
}
void ClassProp::setSave(bool val)
{
    if (val == _save)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setSaveImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "save",QVariant(_save), QVariant(val));
    cmd->setText("Set Save");
    Undoer::instance()->push(cmd);
}
void ClassProp::setSaveImp(bool val)
{
    _save = val;
    emit saveChanged();
}

// ----[ notify ] ----
bool ClassProp::notify() const
{
    return _notify;
}
void ClassProp::setNotify(bool val)
{
    if (val == _notify)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setNotifyImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "notify",QVariant(_notify), QVariant(val));
    cmd->setText("Set Notify");
    Undoer::instance()->push(cmd);
}
void ClassProp::setNotifyImp(bool val)
{
    _notify = val;
    emit notifyChanged();
}

// ----[ undo ] ----
bool ClassProp::undo() const
{
    return _undo;
}
void ClassProp::setUndo(bool val)
{
    if (val == _undo)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setUndoImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "undo",QVariant(_undo), QVariant(val));
    cmd->setText("Set Undo");
    Undoer::instance()->push(cmd);
}
void ClassProp::setUndoImp(bool val)
{
    _undo = val;
    emit undoChanged();
}


// ----[ null ] ----
bool ClassProp::null() const
{
    return _null;
}
void ClassProp::setNull(bool val)
{
    if (val == _null)
        return;


    _null = val;
    emit nullChanged();

    updateInit();
}


// ----[ validate ] ----
QString ClassProp::validate() const
{
    return _validate;
}
void ClassProp::setValidate(QString val)
{
    if (val == _validate)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setValidateImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "validate",QVariant(_validate), QVariant(val));
    cmd->setText("Set Validate");
    Undoer::instance()->push(cmd);
}
void ClassProp::setValidateImp(QString val)
{
    _validate = val;
    emit validateChanged();
}

// ----[ destruct ] ----
QString ClassProp::destruct() const
{
    return _destruct;
}
void ClassProp::setDestruct(QString val)
{
    if (val == _destruct)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setDestructImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "destruct",QVariant(_destruct), QVariant(val));
    cmd->setText("Set Destruct");
    Undoer::instance()->push(cmd);
}
void ClassProp::setDestructImp(QString val)
{
    _destruct = val;
    emit destructChanged();
}

// ----[ name ] ----
QString ClassProp::name() const
{
    return _name;
}
void ClassProp::setName(QString val)
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
void ClassProp::setNameImp(QString val)
{
    updateInit();

    _name = val;
    emit nameChanged();
}

// ----[ type ] ----
QString ClassProp::type() const
{
    return _type;
}
void ClassProp::setType(QString val)
{
    if (val == _type)
        return;


    if (Undoer::instance()->noundo() == false)
    {
        setTypeImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "type",QVariant(_type), QVariant(val));
    cmd->setText("Set Type");
    Undoer::instance()->push(cmd);
}
void ClassProp::setTypeImp(QString val)
{
    _type = val;
    emit typeChanged();

    updateInit();
}

// ----[ subtype ] ----
QString ClassProp::subType() const
{
    return _subType;
}
void ClassProp::setSubType(QString val)
{
    if (val == _subType)
        return;



    if (Undoer::instance()->noundo() == false)
    {
        setSubTypeImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "subType",QVariant(_subType), QVariant(val));
    cmd->setText("Set Type");
    Undoer::instance()->push(cmd);
}
void ClassProp::setSubTypeImp(QString val)
{
    _subType = val;
    emit subTypeChanged();

    updateInit();
}

// ----[ subtype ] ----
QPointF ClassProp::cpos() const
{
    return _cpos;
}
void ClassProp::setCPos(QPointF val)
{
    if (val == _cpos)
        return;

    setCPosImp(val);

}
void ClassProp::setCPosImp(QPointF val)
{
    _cpos = val;
    emit cposChanged();
}

// ----[ init ] ----
QString ClassProp::init() const
{
    return _init;
}
void ClassProp::setInit(QString val)
{
    if (val == _init)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setInitImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "init",QVariant(_init), QVariant(val));
    cmd->setText("Set Init");
    Undoer::instance()->push(cmd);
}
void ClassProp::setInitImp(QString val)
{
    _init = val;
    emit initChanged();
}

// ----[ write ] ----
bool ClassProp::write() const
{
    return _write;
}
void ClassProp::setWrite(bool val)
{
    if (val == _write)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setWriteImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "write",QVariant(_write), QVariant(val));
    cmd->setText("Set Write");
    Undoer::instance()->push(cmd);
}
void ClassProp::setWriteImp(bool val)
{
    _write = val;
    emit writeChanged();
}


int ClassProp::count() const
{
    return _count;
}
void ClassProp::setCount(int val)
{
    if (_count == val)
        return;

    _count = val;

    emit countChanged();

    updateInit();
}

void ClassProp::updateInit()
{
    if (_type == "QObject*")
    {
        if (_null)
        {
            setInit( QString("_") + _name + " = NULL;");
            setDestruct("");
        }
        else
        {
            QString Name = _name;
            QChar *ptr = Name.data();
            *ptr = ptr->toUpper();

            QString init = \
                    QString("_") + _name + " = NULL;\n"
                    "\tset%%Propname%%Imp(new " + _subType + "());\n";

            init.replace("%%Propname%%", Name, Qt::CaseSensitive);

            setInit(init);
            setDestruct(QString("_") + _name+ "->deleteLater();");
        }
    }

    if (_type == "ObjectList*")
    {
        setInit( (QString("_") + _name + " = new ObjectList("+_subType+"::staticMetaObject, %1);").arg(_count));
        setDestruct(QString("_") + _name+ "->deleteLater();");
    }



}
QDataStream& operator<< (QDataStream& ds, const ClassProp * p)
{


    // ----[ read SAVE ] ----
    ds << p->_read;


    // ----[ save SAVE ] ----
    ds << p->_save;


    // ----[ notify SAVE ] ----
    ds << p->_notify;


    // ----[ undo SAVE ] ----
    ds << p->_undo;


    // ----[ validate SAVE ] ----
    ds << p->_validate;


    // ----[ destruct SAVE ] ----
    ds << p->_destruct;


    // ----[ name SAVE ] ----
    ds << p->_name;


    // ----[ type SAVE ] ----
    ds << p->_type;
    ds << p->_subType;


    // ----[ init SAVE ] ----
    ds << p->_init;


    // ----[ write SAVE ] ----
    ds << p->_write;

    ds << p->_count;

    ds << p->null();

    return ds;
}

QDataStream& operator>> (QDataStream& ds, ClassProp * p)
{
    quint32 count;
    quint32 index;

    // ----[ read LOAD ] ----
    ds >> p->_read;

    // ----[ save LOAD ] ----
    ds >> p->_save;

    // ----[ notify LOAD ] ----
    ds >> p->_notify;

    // ----[ undo LOAD ] ----
    ds >> p->_undo;

    // ----[ validate LOAD ] ----
    ds >> p->_validate;

    // ----[ destruct LOAD ] ----
    ds >> p->_destruct;

    // ----[ name LOAD ] ----
    ds >> p->_name;

    // ----[ type LOAD ] ----
    ds >> p->_type;
    ds >> p->_subType;

    // ----[ init LOAD ] ----
    ds >> p->_init;

    // ----[ write LOAD ] ----
    ds >> p->_write;

    ds >> p->_count;

    ds >> p->_null;


    return ds;
}





























QString ClassProp::generateH() const
{
    QString h;

    h += "public:\n";

    h += "\tQ_PROPERTY(%%proptype%% %%propname%% ";
    if (_read)
        h += " READ %%propname%% ";
    if (_write)
        h += " WRITE set%%Propname%% ";

    if (_notify)
        h += " NOTIFY %%propname%%Changed ";
    if (!_write)
        h += " CONSTANT ";
    h += " )\n";

    if (_validate.length() > 0)
        h += "\tbool validate%%Propname%%(%%proptype%% val) const;\n";

    if (_read)
        h += "\t%%proptype%% %%propname%%() const;\n";

    if (_write)
        h += "\tvoid set%%Propname%%(%%proptype%% val);\n";

    if (_notify)
        h += "signals:\n\tvoid %%propname%%Changed();\n";

    h += "private:\n\tvoid set%%Propname%%Imp(%%proptype%% val);\n";

    h += "private:\n";
    h += "\t%%proptype%% _%%propname%%;\n";

    if (_type == "QObject*")
    {
        h += "private slots:\n"
                "\tvoid %%propname%%DeletedSlot();\n";
    }
    QString Name = _name;
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = _name;
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = _type;
    if (type == "QObject*")
        type = _subType + "*";// pointer to subclass

    h.replace("%%proptype%%", type, Qt::CaseSensitive);
    h.replace("%%propname%%", name, Qt::CaseSensitive);
    h.replace("%%Propname%%", Name, Qt::CaseSensitive);

    return h;
}

QString ClassProp::generateCPP(const QString &classname) const
{
    QString cpp;

    ////////////////////////////////////////////////////////////
    cpp += ""
            "%%proptype%% %%Classname%%::%%propname%%() const\n"
            "{\n"
            "\treturn _%%propname%%;\n"
            "}\n";


    ////////////////////////////////////////////////////////////
    if (_write)
    {
        cpp +=  "void %%Classname%%::set%%Propname%%(%%proptype%% val)\n"
                "{\n";

        cpp += "\tif (val == _%%propname%%)\n"
                "\t\treturn;\n\n";

        if (_undo)
        {
            cpp += "\tif (Undoer::instance()->noundo() == false)\n"
                    "\t{\n"
                    "\t\tset%%Propname%%Imp(val);\n"
                    "\t\treturn;\n"
                    "\t}\n\n";
        }

        if (_validate.length() > 0)
            cpp += "\tif (!validate%%Propname%%(val))\n"
                    "\t\treturn;\n\n";

        if (_undo)
        {
            cpp += "\tQUndoCommand *cmd = new PropertyChangeCmd(this, \"%%propname%%\",";

            if (!_type.endsWith("*")) // if its not a pointer
                cpp += "QVariant(_%%propname%%), QVariant(val));\n";
            else
                cpp += "QVariant::fromValue(_%%propname%%), QVariant::fromValue(val));\n";

            cpp += "\tcmd->setText(\"Set %%Propname%%\");\n"
                    "\tUndoer::instance()->push(cmd);\n";
        }
        else if (_notify) // if no undo
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
    if (_type == "QObject*")
    {
        cpp += "\tif (_%%propname%% != NULL)\n"
                "\t\tdisconnect(reinterpret_cast<QObject*>(_%%propname%%), SIGNAL(destroyed()), this, SLOT(%%propname%%DeletedSlot()));\n";
        cpp += "\tif (val != NULL)\n"
                "\t\tconnect(reinterpret_cast<QObject*>(val), SIGNAL(destroyed()), this, SLOT(%%propname%%DeletedSlot()));\n\n";
    }
    cpp += "\t_%%propname%% = val;\n";
    if (_notify)
        cpp += "\temit %%propname%%Changed();\n";
    cpp +=  "}\n";

    ////////////////////////////////////////////////////////////////////////
    if (_validate > 0 && _write)
    {
        cpp += "bool %%Classname%%::validate%%Propname%%(%%proptype%% val) const\n"
                "{\n"
                "\treturn %%validate%%;\n"
                "}\n";
    }
    ////////////////////////////////////////////////////////////////////////
    if (_type == "QObject*")
    {
        cpp += "void %%Classname%%::%%propname%%DeletedSlot()\n"
                "{\n"
                "\tset%%Propname%%(NULL);\n"
                "}\n";
    }

    ////////////////////////////////////////////////////////////////////////

    QString Name = _name;
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = _name;
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = _type;
    if (type == "QObject*")
        type = _subType + "*";// pointer to subclass

    cpp.replace("%%validate%%", _validate, Qt::CaseSensitive);
    cpp.replace("%%Classname%%", classname, Qt::CaseSensitive);
    cpp.replace("%%propname%%", name, Qt::CaseSensitive);
    cpp.replace("%%Propname%%", Name, Qt::CaseSensitive);
    cpp.replace("%%proptype%%", type, Qt::CaseSensitive);

    return cpp;
}



QString ClassProp::generateSave() const
{
    QString cpp;

    if (_type == "QObject*")
        cpp += "\tds << %%proptype%%::_indexedPtrs.value(p->_%%propname%%);";
    else if (_type == "ObjectList*")
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

    QString Name = _name;
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = _name;
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = _type;
    if (type == "QObject*")
        type = _subType ;// pointer to subclass
    else if (type == "ObjectList*")
        type = _subType ;// pointer to subclass


    cpp.replace("%%propname%%", name, Qt::CaseSensitive);
    cpp.replace("%%Propname%%", Name, Qt::CaseSensitive);
    cpp.replace("%%proptype%%", type, Qt::CaseSensitive);


    return cpp;
}


QString ClassProp::generateLoad() const
{
    QString cpp;

    if (_type == "QObject*")
        cpp += "\tds >> index; p->set%%Propname%%Imp((index == -1 ? NULL : %%proptype%%::_ptrs[index]));\n";
    else if (_type == "ObjectList*")
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



    QString Name = _name;
    QChar *ptr = Name.data();
    *ptr = ptr->toUpper();

    QString name = _name;
    ptr = name.data();
    *ptr = ptr->toLower();

    QString type = _type;
    if (type == "QObject*")
        type = _subType ;// pointer to subclass
    else if (type == "ObjectList*")
        type = _subType ;// pointer to subclass


    cpp.replace("%%propname%%", name, Qt::CaseSensitive);
    cpp.replace("%%Propname%%", Name, Qt::CaseSensitive);
    cpp.replace("%%proptype%%", type, Qt::CaseSensitive);


    return cpp;
}

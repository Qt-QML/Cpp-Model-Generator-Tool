#include "classProp.h"
#include <QQmlEngine>

//// - [ static ] ----------------------------------------------------------------------------
QList<ClassProp*> ClassProp::_ptrs;
QHash<ClassProp *, quint32> ClassProp::_indexedPtrs;

void ClassProp::init(int count)
{
    //clear current
    deleteAll();

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
    while (i.hasNext())        _indexedPtrs.insert(i.next(), index++);
}

void ClassProp::clearIndex()
{
    _indexedPtrs.clear();
}

void ClassProp::deleteAll()
{
    QList<ClassProp*> tmp = _ptrs;
    _ptrs.clear();
    for (int i=0;i<tmp.count();i++)
        delete tmp[i];
}

//// - [ non-static ] ----------------------------------------------------------------------------

ClassProp::ClassProp(QObject *parent) :
    QObject(parent)
{
    _name = "unnamed";
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
}

// ----[ subType ] ----
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
    cmd->setText("Set SubType");
    Undoer::instance()->push(cmd);
}
void ClassProp::setSubTypeImp(QString val)
{
    _subType = val;
    emit subTypeChanged();
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

// ----[ count ] ----
qint32 ClassProp::count() const
{
    return _count;
}
void ClassProp::setCount(qint32 val)
{
    if (val == _count)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setCountImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "count",QVariant(_count), QVariant(val));
    cmd->setText("Set Count");
    Undoer::instance()->push(cmd);
}
void ClassProp::setCountImp(qint32 val)
{
    _count = val;
    emit countChanged();
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

    if (Undoer::instance()->noundo() == false)
    {
        setNullImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "null",QVariant(_null), QVariant(val));
    cmd->setText("Set Null");
    Undoer::instance()->push(cmd);
}
void ClassProp::setNullImp(bool val)
{
    _null = val;
    emit nullChanged();
}

// ----[ cpos ] ----
QPointF ClassProp::cpos() const
{
    return _cpos;
}
void ClassProp::setCpos(QPointF val)
{
    if (val == _cpos)
        return;

    _cpos = val;
    emit cposChanged();
}
void ClassProp::setCposImp(QPointF val)
{
    _cpos = val;
    emit cposChanged();
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


    // ----[ subType SAVE ] ----
    ds << p->_subType;


    // ----[ init SAVE ] ----
    ds << p->_init;


    // ----[ write SAVE ] ----
    ds << p->_write;


    // ----[ count SAVE ] ----
    ds << p->_count;


    // ----[ null SAVE ] ----
    ds << p->_null;


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

    // ----[ subType LOAD ] ----
    ds >> p->_subType;

    // ----[ init LOAD ] ----
    ds >> p->_init;

    // ----[ write LOAD ] ----
    ds >> p->_write;

    // ----[ count LOAD ] ----
    ds >> p->_count;

    // ----[ null LOAD ] ----
    ds >> p->_null;


    return ds;
}

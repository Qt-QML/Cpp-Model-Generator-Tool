#include "classprop.h"
#include <QQmlEngine>

//// - [ static ] ----------------------------------------------------------------------------
QList<ClassProp*> ClassProp::_ptrs;
QHash<ClassProp *, int> ClassProp::_indexedPtrs;

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

void ClassProp::loadFromJson(const QJsonArray &array)
{
    for (int i = 0; i < _ptrs.size(); ++i)
        fromJson(array.at(i), _ptrs[i]);
}

QJsonArray ClassProp::saveToJson()
{
    QJsonArray array;
    for (auto p : _ptrs)
        array.append(toJson(p));
    return array;
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


QJsonObject toJson(const ClassProp *p)
{
    QJsonObject object;
    object.insert(QLatin1String("read"), toJson(p->_read));
    object.insert(QLatin1String("save"), toJson(p->_save));
    object.insert(QLatin1String("notify"), toJson(p->_notify));
    object.insert(QLatin1String("undo"), toJson(p->_undo));
    object.insert(QLatin1String("validate"), toJson(p->_validate));
    object.insert(QLatin1String("destruct"), toJson(p->_destruct));
    object.insert(QLatin1String("name"), toJson(p->_name));
    object.insert(QLatin1String("type"), toJson(p->_type));
    object.insert(QLatin1String("subType"), toJson(p->_subType));
    object.insert(QLatin1String("init"), toJson(p->_init));
    object.insert(QLatin1String("write"), toJson(p->_write));
    object.insert(QLatin1String("count"), toJson(p->_count));
    object.insert(QLatin1String("null"), toJson(p->_null));
    return object;
}

void fromJson(const QJsonValue &value, ClassProp *p)
{
    const QJsonObject object = value.toObject();
    fromJson(object.value(QLatin1String("read")), p->_read);
    fromJson(object.value(QLatin1String("save")), p->_save);
    fromJson(object.value(QLatin1String("notify")), p->_notify);
    fromJson(object.value(QLatin1String("undo")), p->_undo);
    fromJson(object.value(QLatin1String("validate")), p->_validate);
    fromJson(object.value(QLatin1String("destruct")), p->_destruct);
    fromJson(object.value(QLatin1String("name")), p->_name);
    fromJson(object.value(QLatin1String("type")), p->_type);
    fromJson(object.value(QLatin1String("subType")), p->_subType);
    fromJson(object.value(QLatin1String("init")), p->_init);
    fromJson(object.value(QLatin1String("write")), p->_write);
    fromJson(object.value(QLatin1String("count")), p->_count);
    fromJson(object.value(QLatin1String("null")), p->_null);
}

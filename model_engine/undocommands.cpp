#include "undocommands.h"
#include <QDebug>

PropertyChangeCmd::PropertyChangeCmd(QObject *obj, const QByteArray & name, const QVariant &val, const QVariant &newval)
    :
      _obj(obj),
      _name(name),
      _val(val),
      _newval(newval)
{
    _time.start();

    connect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    if (_newval.userType() >= QMetaType::User)
        if (qvariant_cast<QObject *>(_newval))
            connect( qvariant_cast<QObject *>(_newval), SIGNAL(destroyed()), this, SLOT(objDestroyed()));
    if (_val.userType() >= QMetaType::User)
        if (qvariant_cast<QObject *>(_val))
            connect( qvariant_cast<QObject *>(_val), SIGNAL(destroyed()), this, SLOT(objDestroyed()));
}

void PropertyChangeCmd::objDestroyed()
{
    if (_obj)
        disconnect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    if (_newval.userType() >= QMetaType::User)
        disconnect( qvariant_cast<QObject *>(_newval), SIGNAL(destroyed()), this, SLOT(objDestroyed()));
    if (_val.userType() >= QMetaType::User)
        disconnect( qvariant_cast<QObject *>(_val), SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    qDebug() << "PropertyChangeCmd::objDestroyed";

    setText("null");
    _obj = Q_NULLPTR;
    //now this Cmd does nothing
}

PropertyChangeCmd::~PropertyChangeCmd()
{
}

void PropertyChangeCmd::undo()
{
    if (_obj)
        _obj->setProperty(_name.data(), _val);
}

void PropertyChangeCmd::redo()
{
    if (_obj)
        _obj->setProperty(_name.data(), _newval);
}

bool PropertyChangeCmd::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) // make sure other is also an AppendText command
        return false;

    const PropertyChangeCmd* o = static_cast<const PropertyChangeCmd*>(other);
    if (o->_name != _name)
        return false;

    if (o->_obj != _obj)
        return false;

    if(_time.elapsed() > 500)
        return false;

    _time.restart();

    _newval = o->_newval;

    return true;
}
int PropertyChangeCmd::id() const
{
    return 1;
}


RemoveRowCmd::RemoveRowCmd(ObjectList *model, int index)
    :
      _obj(Q_NULLPTR),
      _model(model),
      _index(index)
{
    setText("Remove row");
}

RemoveRowCmd::~RemoveRowCmd()
{
}

void RemoveRowCmd::undo()
{
    if (_obj)
        _model->insertRow(_obj, _index);
}

void RemoveRowCmd::redo()
{
    if (_obj)
        disconnect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    _obj = _model->get(_index);
    _model->removeRow(_index);

    connect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));
}

void RemoveRowCmd::objDestroyed()
{
    disconnect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    qDebug() << "RemoveRowCmd::objDestroyed";

    setText("null");

    _obj = Q_NULLPTR;
    //now this Cmd does nothing
}





MoveRowCmd::MoveRowCmd(ObjectList *model, int index, int toIndex)
    :
      _obj1(Q_NULLPTR),
      _obj2(Q_NULLPTR),
      _model(model),
      _index(index),
      _toIndex(toIndex)
{
    setText("Move row");

    _time.start();

    _obj1 = model->get<QObject*>(_index);
    _obj2 = model->get<QObject*>(_toIndex);

    connect(_obj1, SIGNAL(destroyed()), this, SLOT(objDestroyed()));
    connect(_obj2, SIGNAL(destroyed()), this, SLOT(objDestroyed()));
}

MoveRowCmd::~MoveRowCmd()
{
}

void MoveRowCmd::undo()
{
    if (_obj1 && _obj2)
        _model->moveRows(QModelIndex(), _toIndex, 1, QModelIndex(), _index);
}

void MoveRowCmd::redo()
{
    if (_obj1 && _obj2)
        _model->moveRows(QModelIndex(), _index, 1,  QModelIndex(), _toIndex);
}

void MoveRowCmd::objDestroyed()
{
    disconnect(_obj1, SIGNAL(destroyed()), this, SLOT(objDestroyed()));
    disconnect(_obj2, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    setText("null");

    _obj1 = Q_NULLPTR;
    _obj2 = Q_NULLPTR;
}

bool MoveRowCmd::mergeWith(const QUndoCommand *other)
{
    if (other->id() != id()) // make sure other is also an AppendText command
        return false;

    const MoveRowCmd* o = static_cast<const MoveRowCmd*>(other);

    if (o->_model != _model)
        return false;

    if(_time.elapsed() > 500)
        return false;

    _time.restart();

    _toIndex = o->_toIndex;

    return true;
}
int MoveRowCmd::id() const
{
    return 2;
}







InsertRowCmd::InsertRowCmd(ObjectList *model, QObject *obj, int index)
    :
      _obj(obj),
      _model(model),
      _index(index)
{
    connect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    setText("insert row");
}

InsertRowCmd::~InsertRowCmd()
{
}

void InsertRowCmd::undo()
{
    if (_obj)
        _model->removeRow(_index);
}

void InsertRowCmd::redo()
{
    if (_obj)
        _model->insertRow(_obj, _index);
}

void InsertRowCmd::objDestroyed()
{
    disconnect(_obj, SIGNAL(destroyed()), this, SLOT(objDestroyed()));

    qDebug() << "InsertRowCmd::objDestroyed";

    setText("null");

    _obj = Q_NULLPTR;
    //now this Cmd does nothing
}

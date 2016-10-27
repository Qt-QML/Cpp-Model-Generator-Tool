#include "enum.h"
#include <QQmlEngine>

//// - [ static ] ----------------------------------------------------------------------------
QList<Enum*> Enum::_ptrs;
QHash<Enum *, int> Enum::_indexedPtrs;

void Enum::init(int count)
{
    //clear current
    deleteAll();

    for (int i=0;i<count;i++)
        new Enum;
}

void Enum::load(QDataStream& ds)
{
    quint32 count = _ptrs.count();
    for (quint32 i=0;i<count; i++)
        ds >> _ptrs[i];
}

void Enum::save(QDataStream& ds)
{
    QListIterator<Enum *> i(_ptrs);
    while (i.hasNext())
        ds << i.next();
}

void Enum::loadFromJson(const QJsonArray &array)
{
    for (int i = 0; i < _ptrs.size(); ++i)
        fromJson(array.at(i), _ptrs[i]);
}

QJsonArray Enum::saveToJson()
{
    QJsonArray array;
    for (auto p : _ptrs)
        array.append(toJson(p));
    return array;
}

void Enum::createIndex() //must be called for all classes before save for this class
{
    _indexedPtrs.clear();
    _indexedPtrs.insert(NULL, -1);
    quint32 index = 0;
    QListIterator<Enum *> i(_ptrs);
    while (i.hasNext())        _indexedPtrs.insert(i.next(), index++);
}

void Enum::clearIndex()
{
    _indexedPtrs.clear();
}

void Enum::deleteAll()
{
    QList<Enum*> tmp = _ptrs;
    _ptrs.clear();
    for (int i=0;i<tmp.count();i++)
        delete tmp[i];
}

//// - [ non-static ] ----------------------------------------------------------------------------

Enum::Enum(QObject *parent) :
    QObject(parent)
{
    _name = "unnamed";
    _values = new ObjectList(EnumValue::staticMetaObject, 0);

    _ptrs.append(this);
}

Enum::~Enum()
{
    _values->deleteLater();

    _ptrs.removeOne(this);
}


// ----[ name ] ----
QString Enum::name() const
{
    return _name;
}
void Enum::setName(QString val)
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
void Enum::setNameImp(QString val)
{
    _name = val;
    emit nameChanged();
}

// ----[ values ] ----
ObjectList* Enum::values() const
{
    return _values;
}
void Enum::setValues(ObjectList* val)
{
    if (val == _values)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setValuesImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "values",QVariant::fromValue(_values), QVariant::fromValue(val));
    cmd->setText("Set Values");
    Undoer::instance()->push(cmd);
}
void Enum::setValuesImp(ObjectList* val)
{
    _values = val;
    emit valuesChanged();
}


QDataStream& operator<< (QDataStream& ds, const Enum * p)
{


    // ----[ name SAVE ] ----
    ds << p->_name;


    // ----[ values SAVE ] ----
    ds << ((qint32)(p->_values->count()));
    for (int i=0; i<p->_values->count(); i++)
    {
        EnumValue *o = p->_values->get<EnumValue*>(i);
        ds << EnumValue::_indexedPtrs.value(o);
    }


    return ds;
}

QDataStream& operator>> (QDataStream& ds, Enum * p)
{
    quint32 count;
    quint32 index;

    // ----[ name LOAD ] ----
    ds >> p->_name;

    // ----[ values LOAD ] ----
    p->values()->removeAll();
    ds >> count;
    for (quint32 i=0; i<count; i++)
    {
        ds >> index;
        EnumValue *obj = (index == -1 ? Q_NULLPTR : EnumValue::_ptrs[index]);
        p->values()->insertRow(obj,  p->values()->count());
    }


    return ds;
}


QJsonObject toJson(const Enum *p)
{
    QJsonObject object;
    object.insert(QLatin1String("name"), toJson(p->_name));
    {
        QJsonArray array;
        for (QObject *o : p->_values->list())
            array.append(EnumValue::_indexedPtrs.value(static_cast<EnumValue*>(o)));
        object.insert(QLatin1String("values"), array);
    }
    return object;
}

void fromJson(const QJsonValue &value, Enum *p)
{
    const QJsonObject object = value.toObject();
    fromJson(object.value(QLatin1String("name")), p->_name);
    p->_values->removeAll();
    for (const QJsonValue &value : object.value("values").toArray()) {
        int index = value.toInt(-1);
        EnumValue *obj = (index == -1 ? Q_NULLPTR : EnumValue::_ptrs[index]);
        p->values()->insertRow(obj, p->_values->count());
    }
}

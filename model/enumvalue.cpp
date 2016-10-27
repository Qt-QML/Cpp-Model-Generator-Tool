#include "enumvalue.h"
#include <QQmlEngine>

//// - [ static ] ----------------------------------------------------------------------------
QList<EnumValue*> EnumValue::_ptrs;
QHash<EnumValue *, int> EnumValue::_indexedPtrs;

void EnumValue::init(int count)
{
    //clear current
    deleteAll();

    for (int i=0;i<count;i++)
        new EnumValue;
}

void EnumValue::load(QDataStream& ds)
{
    quint32 count = _ptrs.count();
    for (quint32 i=0;i<count; i++)
        ds >> _ptrs[i];
}

void EnumValue::save(QDataStream& ds)
{
    QListIterator<EnumValue *> i(_ptrs);
    while (i.hasNext())
        ds << i.next();
}

void EnumValue::loadFromJson(const QJsonArray &array)
{
    for (int i = 0; i < _ptrs.size(); ++i)
        fromJson(array.at(i), _ptrs[i]);
}

QJsonArray EnumValue::saveToJson()
{
    QJsonArray array;
    for (auto p : _ptrs)
        array.append(toJson(p));
    return array;
}

void EnumValue::createIndex() //must be called for all classes before save for this class
{
    _indexedPtrs.clear();
    _indexedPtrs.insert(NULL, -1);
    quint32 index = 0;
    QListIterator<EnumValue *> i(_ptrs);
    while (i.hasNext())        _indexedPtrs.insert(i.next(), index++);
}

void EnumValue::clearIndex()
{
    _indexedPtrs.clear();
}

void EnumValue::deleteAll()
{
    QList<EnumValue*> tmp = _ptrs;
    _ptrs.clear();
    for (int i=0;i<tmp.count();i++)
        delete tmp[i];
}

//// - [ non-static ] ----------------------------------------------------------------------------

EnumValue::EnumValue(QObject *parent) :
    QObject(parent)
{
    _name = "unnamed";

    _ptrs.append(this);
}

EnumValue::~EnumValue()
{

    _ptrs.removeOne(this);
}


// ----[ name ] ----
QString EnumValue::name() const
{
    return _name;
}
void EnumValue::setName(QString val)
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
void EnumValue::setNameImp(QString val)
{
    _name = val;
    emit nameChanged();
}


QDataStream& operator<< (QDataStream& ds, const EnumValue * p)
{


    // ----[ name SAVE ] ----
    ds << p->_name;


    return ds;
}

QDataStream& operator>> (QDataStream& ds, EnumValue * p)
{
    quint32 count;
    quint32 index;

    // ----[ name LOAD ] ----
    ds >> p->_name;


    return ds;
}


QJsonObject toJson(const EnumValue *p)
{
    QJsonObject object;
    object.insert(QLatin1String("name"), toJson(p->_name));
    return object;
}

void fromJson(const QJsonValue &value, EnumValue *p)
{
    const QJsonObject object = value.toObject();
    fromJson(object.value(QLatin1String("name")), p->_name);
}

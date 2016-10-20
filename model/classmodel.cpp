#include "classmodel.h"
#include <QQmlEngine>

//// - [ static ] ----------------------------------------------------------------------------
QList<ClassModel*> ClassModel::_ptrs;
QHash<ClassModel *, int> ClassModel::_indexedPtrs;

void ClassModel::init(int count)
{
    //clear current
    deleteAll();

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

void ClassModel::loadFromJson(const QJsonArray &array)
{
    for (int i = 0; i < _ptrs.size(); ++i)
        fromJson(array.at(i), _ptrs[i]);
}

QJsonArray ClassModel::saveToJson()
{
    QJsonArray array;
    for (auto p : _ptrs)
        array.append(toJson(p));
    return array;
}

void ClassModel::createIndex() //must be called for all classes before save for this class
{
    _indexedPtrs.clear();
    _indexedPtrs.insert(NULL, -1);
    quint32 index = 0;
    QListIterator<ClassModel *> i(_ptrs);
    while (i.hasNext())        _indexedPtrs.insert(i.next(), index++);
}

void ClassModel::clearIndex()
{
    _indexedPtrs.clear();
}

void ClassModel::deleteAll()
{
    QList<ClassModel*> tmp = _ptrs;
    _ptrs.clear();
    for (int i=0;i<tmp.count();i++)
        delete tmp[i];
}

//// - [ non-static ] ----------------------------------------------------------------------------

ClassModel::ClassModel(QObject *parent) :
    QObject(parent)
{
    _properties = new ObjectList(ClassProp::staticMetaObject, 0);
    _name = "unnamed";

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
    emit posChanged();
}

// ----[ cpos ] ----
QPointF ClassModel::cpos() const
{
    return _cpos;
}
void ClassModel::setCpos(QPointF val)
{
    if (val == _cpos)
        return;

    _cpos = val;
    emit cposChanged();
}
void ClassModel::setCposImp(QPointF val)
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
        ClassProp *obj = (index == -1 ? Q_NULLPTR : ClassProp::_ptrs[index]);
        p->properties()->insertRow(obj,  p->properties()->count());
    }

    // ----[ name LOAD ] ----
    ds >> p->_name;

    // ----[ pos LOAD ] ----
    ds >> p->_pos;


    return ds;
}


QJsonObject toJson(const ClassModel *p)
{
    QJsonObject object;
    {
        QJsonArray array;
        for (QObject *o : p->_properties->list())
            array.append(ClassProp::_indexedPtrs.value(static_cast<ClassProp*>(o)));
        object.insert(QLatin1String("properties"), array);
    }
    object.insert(QLatin1String("name"), toJson(p->_name));
    object.insert(QLatin1String("pos"), toJson(p->_pos));
    return object;
}

void fromJson(const QJsonValue &value, ClassModel *p)
{
    const QJsonObject object = value.toObject();
    p->_properties->removeAll();
    for (const QJsonValue &value : object.value("properties").toArray()) {
        int index = value.toInt(-1);
        ClassProp *obj = (index == -1 ? Q_NULLPTR : ClassProp::_ptrs[index]);
        p->properties()->insertRow(obj, p->_properties->count());
    }
    fromJson(object.value(QLatin1String("name")), p->_name);
    fromJson(object.value(QLatin1String("pos")), p->_pos);
}

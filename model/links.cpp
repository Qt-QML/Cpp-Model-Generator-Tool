#include "links.h"
#include <QQmlEngine>

//// - [ static ] ----------------------------------------------------------------------------
QList<Links*> Links::_ptrs;
QHash<Links *, int> Links::_indexedPtrs;

void Links::init(int count)
{
    //clear current
    deleteAll();

    for (int i=0;i<count;i++)
        new Links;
}

void Links::load(QDataStream& ds)
{
    quint32 count = _ptrs.count();
    for (quint32 i=0;i<count; i++)
        ds >> _ptrs[i];
}

void Links::save(QDataStream& ds)
{
    QListIterator<Links *> i(_ptrs);
    while (i.hasNext())
        ds << i.next();
}

void Links::loadFromJson(const QJsonArray &array)
{
    for (int i = 0; i < _ptrs.size(); ++i)
        fromJson(array.at(i), _ptrs[i]);
}

QJsonArray Links::saveToJson()
{
    QJsonArray array;
    for (auto p : _ptrs)
        array.append(toJson(p));
    return array;
}

void Links::createIndex() //must be called for all classes before save for this class
{
    _indexedPtrs.clear();
    _indexedPtrs.insert(NULL, -1);
    quint32 index = 0;
    QListIterator<Links *> i(_ptrs);
    while (i.hasNext())        _indexedPtrs.insert(i.next(), index++);
}

void Links::clearIndex()
{
    _indexedPtrs.clear();
}

void Links::deleteAll()
{
    QList<Links*> tmp = _ptrs;
    _ptrs.clear();
    for (int i=0;i<tmp.count();i++)
        delete tmp[i];
}

//// - [ non-static ] ----------------------------------------------------------------------------

Links::Links(QObject *parent) :
    QObject(parent)
{
    _from = Q_NULLPTR;
    _to = Q_NULLPTR;

    _ptrs.append(this);
}

Links::~Links()
{

    _ptrs.removeOne(this);
}


// ----[ from ] ----
ClassProp* Links::from() const
{
    return _from;
}
void Links::setFrom(ClassProp* val)
{
    if (val == _from)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setFromImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "from",QVariant::fromValue(_from), QVariant::fromValue(val));
    cmd->setText("Set From");
    Undoer::instance()->push(cmd);
}
void Links::setFromImp(ClassProp* val)
{
    if (_from)
        disconnect(_from, SIGNAL(destroyed()), this, SLOT(fromDeletedSlot()));
    if (val)
        connect(val, SIGNAL(destroyed()), this, SLOT(fromDeletedSlot()));

    _from = val;
    emit fromChanged();
}
void Links::fromDeletedSlot()
{
    setFrom(Q_NULLPTR);
}

// ----[ to ] ----
ClassModel* Links::to() const
{
    return _to;
}
void Links::setTo(ClassModel* val)
{
    if (val == _to)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setToImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "to",QVariant::fromValue(_to), QVariant::fromValue(val));
    cmd->setText("Set To");
    Undoer::instance()->push(cmd);
}
void Links::setToImp(ClassModel* val)
{
    if (_to)
        disconnect(_to, SIGNAL(destroyed()), this, SLOT(toDeletedSlot()));
    if (val)
        connect(val, SIGNAL(destroyed()), this, SLOT(toDeletedSlot()));

    _to = val;
    emit toChanged();
}
void Links::toDeletedSlot()
{
    setTo(Q_NULLPTR);
}


QDataStream& operator<< (QDataStream& ds, const Links * p)
{


    // ----[ from SAVE ] ----
    ds << ClassProp::_indexedPtrs.value(p->_from);

    // ----[ to SAVE ] ----
    ds << ClassModel::_indexedPtrs.value(p->_to);

    return ds;
}

QDataStream& operator>> (QDataStream& ds, Links * p)
{
    quint32 count;
    quint32 index;

    // ----[ from LOAD ] ----
    ds >> index; p->setFromImp((index == -1 ? Q_NULLPTR : ClassProp::_ptrs[index]));

    // ----[ to LOAD ] ----
    ds >> index; p->setToImp((index == -1 ? Q_NULLPTR : ClassModel::_ptrs[index]));


    return ds;
}


QJsonObject toJson(const Links *p)
{
    QJsonObject object;
    object.insert(QLatin1String("from"),
                  ClassProp::_indexedPtrs.value(p->_from));
    object.insert(QLatin1String("to"),
                  ClassModel::_indexedPtrs.value(p->_to));
    return object;
}

void fromJson(const QJsonValue &value, Links *p)
{
    const QJsonObject object = value.toObject();
    {
        int index = object.value(QLatin1String("from")).toInt(-1);
        p->setFromImp(index == -1 ? Q_NULLPTR : ClassProp::_ptrs[index]);
    }
    {
        int index = object.value(QLatin1String("to")).toInt(-1);
        p->setToImp(index == -1 ? Q_NULLPTR : ClassModel::_ptrs[index]);
    }
}

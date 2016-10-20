#include "model.h"
#include <QQmlEngine>

Model::Model(QObject *parent) :
    QObject(parent)
{
    _links = new ObjectList(Links::staticMetaObject, 0);
    _classes = new ObjectList(ClassModel::staticMetaObject, 0);;
    _winRect = QRectF(100,100, 600, 400);
}

Model::~Model()
{
    _links->deleteLater();
    _classes->deleteLater();;
}


// ----[ links ] ----
ObjectList* Model::links() const
{
    return _links;
}
void Model::setLinks(ObjectList* val)
{
    if (val == _links)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setLinksImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "links",QVariant::fromValue(_links), QVariant::fromValue(val));
    cmd->setText("Set Links");
    Undoer::instance()->push(cmd);
}
void Model::setLinksImp(ObjectList* val)
{
    _links = val;
    emit linksChanged();
}

// ----[ classes ] ----
ObjectList* Model::classes() const
{
    return _classes;
}
void Model::setClasses(ObjectList* val)
{
    if (val == _classes)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setClassesImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "classes",QVariant::fromValue(_classes), QVariant::fromValue(val));
    cmd->setText("Set Classes");
    Undoer::instance()->push(cmd);
}
void Model::setClassesImp(ObjectList* val)
{
    _classes = val;
    emit classesChanged();
}

// ----[ winRect ] ----
QRectF Model::winRect() const
{
    return _winRect;
}
void Model::setWinRect(QRectF val)
{
    if (val == _winRect)
        return;

    if (Undoer::instance()->noundo() == false)
    {
        setWinRectImp(val);
        return;
    }

    QUndoCommand *cmd = new PropertyChangeCmd(this, "winRect",QVariant(_winRect), QVariant(val));
    cmd->setText("Set WinRect");
    Undoer::instance()->push(cmd);
}
void Model::setWinRectImp(QRectF val)
{
    _winRect = val;
    emit winRectChanged();
}

// ----[ name ] ----
QString Model::name() const
{
    return _name;
}
void Model::setName(QString val)
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
void Model::setNameImp(QString val)
{
    _name = val;
    emit nameChanged();
}


QDataStream& operator<< (QDataStream& ds, const Model * p)
{


    // ----[ links SAVE ] ----
    ds << ((qint32)(p->_links->count()));
    for (int i=0; i<p->_links->count(); i++)
    {
        Links *o = p->_links->get<Links*>(i);
        ds << Links::_indexedPtrs.value(o);
    }


    // ----[ classes SAVE ] ----
    ds << ((qint32)(p->_classes->count()));
    for (int i=0; i<p->_classes->count(); i++)
    {
        ClassModel *o = p->_classes->get<ClassModel*>(i);
        ds << ClassModel::_indexedPtrs.value(o);
    }


    // ----[ winRect SAVE ] ----
    ds << p->_winRect;


    // ----[ name SAVE ] ----
    ds << p->_name;


    return ds;
}

QDataStream& operator>> (QDataStream& ds, Model * p)
{
    quint32 count;
    quint32 index;

    // ----[ links LOAD ] ----
    p->links()->removeAll();
    ds >> count;
    for (quint32 i=0; i<count; i++)
    {
        ds >> index;
        Links *obj = (index == -1 ? Q_NULLPTR : Links::_ptrs[index]);
        p->links()->insertRow(obj,  p->links()->count());
    }

    // ----[ classes LOAD ] ----
    p->classes()->removeAll();
    ds >> count;
    for (quint32 i=0; i<count; i++)
    {
        ds >> index;
        ClassModel *obj = (index == -1 ? Q_NULLPTR : ClassModel::_ptrs[index]);
        p->classes()->insertRow(obj,  p->classes()->count());
    }

    // ----[ winRect LOAD ] ----
    ds >> p->_winRect;

    // ----[ name LOAD ] ----
    ds >> p->_name;


    return ds;
}


QJsonObject toJson(const Model *p)
{
    QJsonObject object;
    {
        QJsonArray array;
        for (QObject *o : p->_links->list())
            array.append(Links::_indexedPtrs.value(static_cast<Links*>(o)));
        object.insert(QLatin1String("links"), array);
    }
    {
        QJsonArray array;
        for (QObject *o : p->_classes->list())
            array.append(ClassModel::_indexedPtrs.value(static_cast<ClassModel*>(o)));
        object.insert(QLatin1String("classes"), array);
    }
    object.insert(QLatin1String("winRect"), toJson(p->_winRect));
    object.insert(QLatin1String("name"), toJson(p->_name));
    return object;
}

void fromJson(const QJsonValue &value, Model *p)
{
    const QJsonObject object = value.toObject();
    p->_links->removeAll();
    for (const QJsonValue &value : object.value("links").toArray()) {
        int index = value.toInt(-1);
        Links *obj = (index == -1 ? Q_NULLPTR : Links::_ptrs[index]);
        p->links()->insertRow(obj, p->_links->count());
    }
    p->_classes->removeAll();
    for (const QJsonValue &value : object.value("classes").toArray()) {
        int index = value.toInt(-1);
        ClassModel *obj = (index == -1 ? Q_NULLPTR : ClassModel::_ptrs[index]);
        p->classes()->insertRow(obj, p->_classes->count());
    }
    fromJson(object.value(QLatin1String("winRect")), p->_winRect);
    fromJson(object.value(QLatin1String("name")), p->_name);
}

#include "objectlist.h"
#include <QQmlEngine>
#include <QDebug>
#include "undoer.h"

ObjectList::ObjectList(QMetaObject info, int count) :
    QAbstractItemModel()
{
    _metaInfo = info;

    for (int i=0;i<count;i++)
        addRowWithUndo();

    emit countChanged();
}

ObjectList::~ObjectList()
{

}

QHash<int, QByteArray> ObjectList::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::UserRole] = "row";
    roleNames[Qt::UserRole+1] = "mod";
    roleNames[Qt::UserRole+2] = "name";
    return roleNames;
}

int ObjectList::columnCount ( const QModelIndex & parent) const
{
    return 1;
}

int ObjectList::rowCount ( const QModelIndex & parent) const
{
    return _list.count();
}

bool ObjectList::hasChildren ( const QModelIndex & parent) const
{
    return false;
}

QModelIndex ObjectList::parent ( const QModelIndex & index ) const
{
    return QModelIndex();
}

Qt::ItemFlags ObjectList::flags ( const QModelIndex & index ) const
{
    return Qt::ItemIsEditable;
}

QModelIndex	ObjectList::index ( int row, int column, const QModelIndex & parent) const
{
    return QAbstractItemModel::createIndex(row,0);
}
QVariant ObjectList::data ( const QModelIndex & index, int role  ) const
{
    if (role == Qt::UserRole) // row
        return qVariantFromValue(_list[index.row()]);
    if (role == (Qt::UserRole+1)) // row
        return qVariantFromValue((ObjectList*)this);
    if (role == (Qt::UserRole+2)) // name
        return _list[index.row()]->property("name");

    return QVariant();
}

//------------------------------------------------------------
// custom model editing -----------------------------------
//------------------------------------------------------------
bool ObjectList::removeRow(int index)
{
    QObject * o = get(index);
    beginRemoveRows(QModelIndex(), index, index);
    {
        disconnect(o, SIGNAL(destroyed(QObject*)), this, SLOT(removeObject_p(QObject*)));
        _list.removeAt(index);
    }
    endRemoveRows();
    //o->deleteLater();//????

    emit countChanged();

    return true;
}

void ObjectList::insertRow(QObject *obj, int index)
{
    beginInsertRows(QModelIndex(), index,index);
    {
        connect(obj, SIGNAL(destroyed(QObject*)), this, SLOT(removeObject_p(QObject*)));
        QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);
        _list.insert(index, obj);
    }
    endInsertRows();

    emit countChanged();
}

bool ObjectList::moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild)
{
    if (sourceRow == destinationChild)
        return false;

    beginMoveRows(sourceParent, sourceRow, sourceRow +count-1, destinationParent, destinationChild + (sourceRow<destinationChild? count : 0));

    QList<QObject*> tmp;

    for (int i=0;i<count;i++)
        tmp.append(_list.takeAt(sourceRow));

    for (int i=0;i<count;i++)
        _list.insert(destinationChild, tmp.takeLast());

    endMoveRows();

    return true;
}
//------------------------------------------------------------
// SLOTS - interface to QML ------------------------------------------------------------
//------------------------------------------------------------
QObject * ObjectList::get(int index) {
    if (index >= rowCount() || index < 0)
    {
        //        if (_empty == NULL)
        //        {
        //            _empty = static_cast<QObject*>(_metaInfo.newInstance());
        //            QQmlEngine::setObjectOwnership(_empty, QQmlEngine::CppOwnership);
        //            //connect(_empty, SIGNAL(destroyed()), this, SLOT(debugDestroy()));
        //        }

        return NULL;
    }
    return _list[index];
}

QVariantList ObjectList::props(const QString & name)
{
    QVariantList list;

    foreach (QObject *obj, _list)
    {
        list.append(obj->property(name.toLatin1().data()));
    }
    return list;
}

void ObjectList::set(int index, const QByteArray & name, const QVariant &value)
{
    QObject *o = get(index);
    if (o)
    {
        if (o->property(name) == value)
            return;

        //push undo/// viga.. setproperty hoopis
        //PropertyChangeCmd *command1 = new PropertyChangeCmd(o, name, o->property(name), value);
        //Undoer::instance()->push(command1);
    }
}

void ObjectList::removeWithUndo(int index)
{
    // execute remove command
    RemoveRowCmd *command1 = new RemoveRowCmd(this, index);
    Undoer::instance()->push(command1);
}

void ObjectList::moveWithUndo(int index, int toIndex)
{
    if (index == toIndex)
        return;

    MoveRowCmd *command1 = new MoveRowCmd(this, index, toIndex);
    Undoer::instance()->push(command1);
}

void ObjectList::removeObject_p(QObject *obj)
{
    removeObject(static_cast<QObject*>(obj));
}

void ObjectList::removeObject(QObject *obj)
{
    int index = _list.indexOf(obj);

    if (index != -1)
        removeRow(index);
}

void ObjectList::removeAll()
{
    while (rowCount())
        removeWithUndo(0);
}

QObject * ObjectList::addRowWithUndo()
{
    //create a new instance from metainfo
    QObject *obj = static_cast<QObject*>(_metaInfo.newInstance());
    QQmlEngine::setObjectOwnership(obj, QQmlEngine::CppOwnership);

    // execute insert command
    InsertRowCmd *command1 = new InsertRowCmd(this, obj, _list.count());
    Undoer::instance()->push(command1);

    return obj;
}

void ObjectList::resize(int size)
{
    removeAll();//slow

    for (int i=0;i<size;i++)
        addRowWithUndo();
}

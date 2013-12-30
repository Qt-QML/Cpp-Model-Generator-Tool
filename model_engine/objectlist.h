#ifndef OBJECTLIST_H
#define OBJECTLIST_H

#include <QAbstractItemModel>

class ObjectList : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ObjectList(QMetaObject info, int count=0); // must be Object derived
    ~ObjectList();

    // basic stuff reimplemented
    virtual int columnCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual int rowCount ( const QModelIndex & parent = QModelIndex() ) const;
    virtual bool hasChildren ( const QModelIndex & parent = QModelIndex() ) const;
    virtual QModelIndex parent ( const QModelIndex & index ) const;
    virtual Qt::ItemFlags flags ( const QModelIndex & index ) const;
    virtual QModelIndex	index ( int row, int column, const QModelIndex & parent = QModelIndex() ) const;
    virtual QVariant data ( const QModelIndex & index, int role = Qt::DisplayRole ) const ;

    QHash<int, QByteArray> roleNames() const;
    ObjectList(QObject *parent = 0) : QAbstractItemModel(parent) {}

    Q_PROPERTY(int count READ count NOTIFY countChanged)
    int count() const { return _list.count(); }

    template <class X>
    QList<X> getProps(const char * name)
    {
        QList<X> list;

        foreach (QObject *obj, _list)
            list.append(qvariant_cast<X>(obj->property(name)));

        return list;
    }
    template <class X>
    X get(int index)
    {
        return qobject_cast<X>(_list.at(index));
    }
    void resize(int size);

    // for undoer
    void insertRow(QObject *obj, int index);
    bool removeRow(int index);
    bool moveRows(const QModelIndex & sourceParent, int sourceRow, int count, const QModelIndex & destinationParent, int destinationChild);

signals:
    void countChanged();

public slots:
    QObject * addRowWithUndo();
    void removeWithUndo(int index);
    void moveWithUndo(int index, int toIndex);

    void removeObject(QObject *obj);
    void removeAll();

    QObject * get(int index);
    void set(int index, const QByteArray & name, const QVariant &value);

    QVariantList props(const QString & name);

private slots:
    void removeObject_p(QObject *obj);

private:
    QList<QObject*> _list;
    QMetaObject _metaInfo;
};

Q_DECLARE_METATYPE(ObjectList*)

#endif // OBJECTLIST_H

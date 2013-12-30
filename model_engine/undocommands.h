#ifndef UNDOCOMMANDS_H
#define UNDOCOMMANDS_H

#include <QUndoCommand>
#include <QObject>
#include <QElapsedTimer>
#include "objectlist.h"


class RemoveRowCmd : public QObject,  public QUndoCommand
{
    Q_OBJECT
public:
    RemoveRowCmd(ObjectList *model, int index);
    ~RemoveRowCmd();

    virtual void undo();
    virtual void redo();

private slots:
    void objDestroyed();

private:
    QObject *_obj;
    ObjectList *_model;
    int _index;
};
class MoveRowCmd : public QObject,  public QUndoCommand
{
    Q_OBJECT
public:
    MoveRowCmd(ObjectList *model, int index, int toIndex);
    ~MoveRowCmd();

    virtual void undo();
    virtual void redo();

    bool mergeWith(const QUndoCommand *other);
    int id() const;

private slots:
    void objDestroyed();

private:
    QObject *_obj1;
    QObject *_obj2;
    ObjectList *_model;
    int _index;
    int _toIndex;
    QElapsedTimer _time;

};

class InsertRowCmd : public QObject,  public QUndoCommand
{
    Q_OBJECT
public:
    InsertRowCmd(ObjectList *model, QObject *obj, int index);
    ~InsertRowCmd();

    virtual void undo();
    virtual void redo();

private slots:
    void objDestroyed();

private:
    QObject *_obj;
    ObjectList *_model;
    int _index;
};


class PropertyChangeCmd : public QObject, public QUndoCommand
{
    Q_OBJECT
public:
    PropertyChangeCmd(QObject *obj, const QByteArray & name, const QVariant &val, const QVariant &newval);
    ~PropertyChangeCmd();

    virtual void undo();
    virtual void redo();

    bool mergeWith(const QUndoCommand *other);
    int id() const;

private slots:
    void objDestroyed();

private:
    QObject *_obj;
    QByteArray _name;
    QVariant _val;
    QVariant _newval;
    QElapsedTimer _time;
};


#endif // UNDOCOMMANDS_H


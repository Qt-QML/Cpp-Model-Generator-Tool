#include "undoer.h"
#include <QDebug>
#include <QQmlEngine>

#define UNDO_MERGE_TIME 100

Undoer *Undoer::_instance = NULL;
Undoer::Undoer(QObject *parent) :
    QObject(parent)
{
    _enabled = true;
    _noundo = true;
    _stack = new QUndoStack(this);

    connect(_stack, SIGNAL(undoTextChanged(QString)), this, SIGNAL(redoTextChanged()));
    connect(_stack, SIGNAL(redoTextChanged(QString)), this, SIGNAL(undoTextChanged()));
}

Undoer * Undoer::instance()
{
    if (!_instance) {
        _instance = new Undoer;
        QQmlEngine::setObjectOwnership(_instance, QQmlEngine::CppOwnership);
    }

    return _instance;
}

QUndoStack * Undoer::stack() const
{
    return _stack;
}

QString Undoer::redoText() const
{
    return _stack->redoText();
}

QString Undoer::undoText() const
{
    return _stack->undoText();
}

void Undoer::push(QUndoCommand *cmd)
{
    _noundo = false;
    {
//        if (_time.elapsed() < UNDO_MERGE_TIME && _stack->count() > 0)
//        {
//            const UndoCommand *lastCommand = reinterpret_cast<const UndoCommand*>(_stack->command(_stack->count()-1));

//            if (cmd->_obj == lastCommand->_obj)
//                cmd->mergeWith(lastCommand);
//            else
//                _stack->push(cmd);
//        }
//        else
//        {
            _stack->push(cmd);
//        }
//        _time.restart();
    }
    _noundo = true;
}

void Undoer::clear()
{
    _stack->clear();
}

bool Undoer::enabled() const
{
    return _enabled;
}
void Undoer::setEnabled(bool val)
{
    if (_enabled == val)
        return;
    _enabled = val;
    emit enabledChanged();
}

void Undoer::undo()
{
    _noundo = false;
    {
        _stack->undo();
    }
    _noundo = true;
}

void Undoer::redo()
{
    _noundo = false;
    {
        _stack->redo();
    }
    _noundo = true;
}

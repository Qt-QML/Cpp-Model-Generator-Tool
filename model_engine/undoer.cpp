#include "undoer.h"
#include <QDebug>

#define UNDO_MERGE_TIME 100

Undoer *Undoer::_instance = NULL;
Undoer::Undoer(QObject *parent) :
    QObject(parent)
{
    _enabled = true;
    _noundo = true;
    _stack = new QUndoStack();
}

Undoer * Undoer::instance()
{
    if (_instance == NULL)
        _instance = new Undoer;

    return _instance;
}

QUndoStack * Undoer::stack() const
{
    return _stack;
}

QString Undoer::nextText()
{
    int index = _stack->index();
    qDebug() <<index;
    return (index < _stack->count()? _stack->command(index)->text() : "");
}

QString Undoer::prevText()
{
    int index = _stack->index()-1;

    return (index >= 0? _stack->command(index)->text() : "");
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

        emit nextTextChanged();
        emit prevTextChanged();
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

        emit nextTextChanged();
        emit prevTextChanged();
    }
    _noundo = true;
}

void Undoer::redo()
{
    _noundo = false;
    {
        _stack->redo();

        emit nextTextChanged();
        emit prevTextChanged();
    }
    _noundo = true;
}
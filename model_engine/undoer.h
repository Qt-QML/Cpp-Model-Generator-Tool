#ifndef UNDOER_H
#define UNDOER_H

#include <QObject>
#include <QVariant>
#include "undocommands.h" //%%%

class Undoer : public QObject
{
    Q_OBJECT
public:

    static Undoer * instance();

    // enabled or should skip
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    bool enabled() const;
    void setEnabled(bool val);

    bool noundo() const { return _noundo; }
    QUndoStack * stack() const;
    void push(QUndoCommand * cmd);

    Q_PROPERTY(QString redoText READ redoText NOTIFY redoTextChanged)
    Q_PROPERTY(QString undoText READ undoText NOTIFY undoTextChanged)

    QString redoText() const;
    QString undoText() const;

signals:
    void enabledChanged();
    void redoTextChanged();
    void undoTextChanged();

public slots:
    void clear();
    void undo();
    void redo();

private:
    explicit Undoer(QObject *parent = 0);

    QUndoStack *_stack;

    static Undoer *_instance;

    bool _enabled;
    bool _noundo;

//    QTime _time;
};




#endif // UNDOER_H

#ifndef %%CLASSNAME%%ROOT_H
#define %%CLASSNAME%%ROOT_H

#include <QObject>

class %%Classname%%;
class Undoer;

class %%Classname%%Root : public QObject
{
    Q_OBJECT
public:
    explicit %%Classname%%Root(QObject *parent = 0);

signals:

public slots:
    QObject * create();
    QObject * load(const QString & fileName);
    void save(const QString & fileName,  QObject * model) const;

    Undoer * undoer() const;
};

#endif // %%CLASSNAME%%

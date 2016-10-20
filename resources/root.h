#ifndef %%CLASSNAME%%LOADER_H
#define %%CLASSNAME%%LOADER_H

#include <QObject>

class %%Classname%%;
class Undoer;

class %%Classname%%Loader : public QObject
{
    Q_OBJECT
public:
    explicit %%Classname%%Loader(QObject *parent = Q_NULLPTR);

signals:

public slots:
    QObject * create();
    QObject * load(const QString & fileName);
    void save(const QString & fileName, QObject *model) const;

    Undoer * undoer() const;
};

#endif // %%CLASSNAME%%LOADER_H

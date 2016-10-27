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

    static void registerTypes();

public slots:
    %%Classname%% *create();
    %%Classname%% *load(const QString &fileName);
    bool save(const QString &fileName, QObject *model) const;

    %%Classname%% *loadFromJson(const QString &fileName);
    bool saveAsJson(const QString &fileName, QObject *model) const;

    Undoer *undoer() const;

private:
    void createIndices() const;
    void clearIndices() const;
};

#endif // %%CLASSNAME%%LOADER_H

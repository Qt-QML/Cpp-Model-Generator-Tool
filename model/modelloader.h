#ifndef MODELLOADER_H
#define MODELLOADER_H

#include <QObject>

class Model;
class Undoer;

class ModelLoader : public QObject
{
    Q_OBJECT

public:
    explicit ModelLoader(QObject *parent = Q_NULLPTR);

    static void registerTypes();

public slots:
    Model *create();
    Model *load(const QString &fileName);
    bool save(const QString &fileName, QObject *model) const;

    Model *loadFromJson(const QString &fileName);
    bool saveAsJson(const QString &fileName, QObject *model) const;

    Undoer *undoer() const;

private:
    void createIndices() const;
    void clearIndices() const;
};

#endif // MODELLOADER_H

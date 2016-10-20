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

signals:

public slots:
    QObject * create();
    QObject * load(const QString & fileName);
    bool save(const QString & fileName, QObject *model) const;

    QObject * loadFromJson(const QString &fileName);
    bool saveAsJson(const QString &fileName, QObject *model) const;

    Undoer * undoer() const;

private:
    void createIndices() const;
    void clearIndices() const;
};

#endif // MODELLOADER_H

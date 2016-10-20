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
    void save(const QString & fileName, QObject *model) const;

    Undoer * undoer() const;
};

#endif // MODELLOADER_H

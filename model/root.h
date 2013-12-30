#ifndef ROOT_H
#define ROOT_H

#include <QObject>
#include "classes.h"

class Root : public QObject
{
    Q_OBJECT
public:
    explicit Root(QObject *parent = 0);

    Q_PROPERTY(Model* model  READ model NOTIFY modelChanged  )
    Q_PROPERTY(Undoer* undoer  READ undoer CONSTANT)


    Undoer * undoer() const;
    Model * model() const;

signals:
    void modelChanged();
    
public slots:
    void open(const QString &fileName);
    void save(const QString &fileName) const;

public slots:
    void generateFiles(const QString &folder) const;

};

#endif // ROOT_H

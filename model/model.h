#ifndef MODEL_H
#define MODEL_H

#include "classes.h"

class Model : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE Model(QObject *parent = 0);
    ~Model();


    // ----[ links ] ----
public:
    Q_PROPERTY(ObjectList* links  READ links  WRITE setLinks  NOTIFY linksChanged  )
    ObjectList* links() const;
    void setLinks(ObjectList* val);
signals:
    void linksChanged();
private:
    void setLinksImp(ObjectList* val);
private:
    ObjectList* _links;


    // ----[ classes ] ----
public:
    Q_PROPERTY(ObjectList* classes  READ classes  WRITE setClasses  NOTIFY classesChanged  )
    ObjectList* classes() const;
    void setClasses(ObjectList* val);
signals:
    void classesChanged();
private:
    void setClassesImp(ObjectList* val);
private:
    ObjectList* _classes;


    // ----[ name ] ----
public:
    Q_PROPERTY(QString name  READ name  WRITE setName  NOTIFY nameChanged  )
    QString name() const;
    void setName(QString val);
signals:
    void nameChanged();
private:
    void setNameImp(QString val);
private:
    QString _name;


    // ----[ winRect ] ----
public:
    Q_PROPERTY(QRectF winRect  READ winRect  WRITE setWinRect  NOTIFY winRectChanged  )
    QRectF winRect() const;
    void setWinRect(QRectF val);
signals:
    void winRectChanged();
private:
    void setWinRectImp(QRectF val);
private:
    QRectF _winRect;

public slots:
    void generateFiles(const QString &folder) const;



public:
    friend QDataStream& operator<< (QDataStream& ds, const Model * p);
    friend QDataStream& operator>> (QDataStream& ds, Model * p);


};
QDataStream& operator<< (QDataStream& ds, const Model * p);
QDataStream& operator>> (QDataStream& ds, Model * p);


#endif // MODEL_H

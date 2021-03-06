#ifndef MODEL_H
#define MODEL_H

#include "classes.h"

class Model : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE Model(QObject *parent = Q_NULLPTR);
    ~Model();


    // ----[ links ] ----
public:
    Q_PROPERTY(ObjectList* links READ links WRITE setLinks NOTIFY linksChanged)
    ObjectList* links() const { return _links; }
    void setLinks(ObjectList* val);
signals:
    void linksChanged();
private:
    void setLinksImp(ObjectList* val);
private:
    ObjectList* _links;


    // ----[ classes ] ----
public:
    Q_PROPERTY(ObjectList* classes READ classes WRITE setClasses NOTIFY classesChanged)
    ObjectList* classes() const { return _classes; }
    void setClasses(ObjectList* val);
signals:
    void classesChanged();
private:
    void setClassesImp(ObjectList* val);
private:
    ObjectList* _classes;


    // ----[ winRect ] ----
public:
    Q_PROPERTY(QRectF winRect READ winRect WRITE setWinRect NOTIFY winRectChanged)
    QRectF winRect() const { return _winRect; }
    void setWinRect(QRectF val);
signals:
    void winRectChanged();
private:
    void setWinRectImp(QRectF val);
private:
    QRectF _winRect;


    // ----[ name ] ----
public:
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    QString name() const { return _name; }
    void setName(QString val);
signals:
    void nameChanged();
private:
    void setNameImp(QString val);
private:
    QString _name;

public:
    friend QDataStream& operator<< (QDataStream& ds, const Model * p);
    friend QDataStream& operator>> (QDataStream& ds, Model * p);

    friend QJsonObject toJson(const Model *p);
    friend void fromJson(const QJsonValue &value, Model *p);

};
QDataStream& operator<< (QDataStream& ds, const Model * p);
QDataStream& operator>> (QDataStream& ds, Model * p);

QJsonObject toJson(const Model *p);
void fromJson(const QJsonValue &value, Model *p);

#endif // MODEL_H

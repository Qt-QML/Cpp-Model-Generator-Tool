#ifndef CLASSMODEL_H
#define CLASSMODEL_H

#include "classes.h"

class ClassModel : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE ClassModel(QObject *parent = Q_NULLPTR);
    ~ClassModel();


    // ----[ properties ] ----
public:
    Q_PROPERTY(ObjectList* properties READ properties WRITE setProperties NOTIFY propertiesChanged)
    ObjectList* properties() const { return _properties; }
    void setProperties(ObjectList* val);
signals:
    void propertiesChanged();
private:
    void setPropertiesImp(ObjectList* val);
private:
    ObjectList* _properties;


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


    // ----[ pos ] ----
public:
    Q_PROPERTY(QPointF pos READ pos WRITE setPos NOTIFY posChanged)
    QPointF pos() const { return _pos; }
    void setPos(QPointF val);
signals:
    void posChanged();
private:
    void setPosImp(QPointF val);
private:
    QPointF _pos;


    // ----[ cpos ] ----
public:
    Q_PROPERTY(QPointF cpos READ cpos WRITE setCpos NOTIFY cposChanged)
    QPointF cpos() const { return _cpos; }
    void setCpos(QPointF val);
signals:
    void cposChanged();
private:
    void setCposImp(QPointF val);
private:
    QPointF _cpos;


    // ----[ enums ] ----
public:
    Q_PROPERTY(ObjectList* enums READ enums WRITE setEnums NOTIFY enumsChanged)
    ObjectList* enums() const { return _enums; }
    void setEnums(ObjectList* val);
signals:
    void enumsChanged();
private:
    void setEnumsImp(ObjectList* val);
private:
    ObjectList* _enums;

public:
    friend QDataStream& operator<< (QDataStream& ds, const ClassModel * p);
    friend QDataStream& operator>> (QDataStream& ds, ClassModel * p);

    friend QJsonObject toJson(const ClassModel *p);
    friend void fromJson(const QJsonValue &value, ClassModel *p);

    static void init(int count);
    static void load(QDataStream& ds);
    static void save(QDataStream& ds);
    static void loadFromJson(const QJsonArray &);
    static QJsonArray saveToJson();
    static void createIndex();
    static void clearIndex();
    static void deleteAll();
    static QList<ClassModel*> _ptrs;
    static QHash<ClassModel*, int> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const ClassModel * p);
QDataStream& operator>> (QDataStream& ds, ClassModel * p);

QJsonObject toJson(const ClassModel *p);
void fromJson(const QJsonValue &value, ClassModel *p);

#endif // CLASSMODEL_H

#ifndef ENUM_H
#define ENUM_H

#include "classes.h"

class Enum : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE Enum(QObject *parent = Q_NULLPTR);
    ~Enum();


    // ----[ name ] ----
public:
    Q_PROPERTY(QString name  READ name  WRITE setName  NOTIFY nameChanged  )
    QString name() const { return _name; }
    void setName(QString val);
signals:
    void nameChanged();
private:
    void setNameImp(QString val);
private:
    QString _name;


    // ----[ values ] ----
public:
    Q_PROPERTY(ObjectList* values  READ values  WRITE setValues  NOTIFY valuesChanged  )
    ObjectList* values() const { return _values; }
    void setValues(ObjectList* val);
signals:
    void valuesChanged();
private:
    void setValuesImp(ObjectList* val);
private:
    ObjectList* _values;

public:
    friend QDataStream& operator<< (QDataStream& ds, const Enum * p);
    friend QDataStream& operator>> (QDataStream& ds, Enum * p);

    friend QJsonObject toJson(const Enum *p);
    friend void fromJson(const QJsonValue &value, Enum *p);

    static void init(int count);
    static void load(QDataStream& ds);
    static void save(QDataStream& ds);
    static void loadFromJson(const QJsonArray &);
    static QJsonArray saveToJson();
    static void createIndex();
    static void clearIndex();
    static void deleteAll();
    static QList<Enum*> _ptrs;
    static QHash<Enum*, int> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const Enum * p);
QDataStream& operator>> (QDataStream& ds, Enum * p);

QJsonObject toJson(const Enum *p);
void fromJson(const QJsonValue &value, Enum *p);

#endif // ENUM_H

#ifndef LINKS_H
#define LINKS_H

#include "classes.h"

class Links : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE Links(QObject *parent = Q_NULLPTR);
    ~Links();


    // ----[ from ] ----
public:
    Q_PROPERTY(ClassProp* from  READ from  WRITE setFrom  NOTIFY fromChanged  )
    ClassProp* from() const { return _from; }
    void setFrom(ClassProp* val);
signals:
    void fromChanged();
private:
    void setFromImp(ClassProp* val);
private:
    ClassProp* _from;
private slots:
    void fromDeletedSlot();


    // ----[ to ] ----
public:
    Q_PROPERTY(ClassModel* to  READ to  WRITE setTo  NOTIFY toChanged  )
    ClassModel* to() const { return _to; }
    void setTo(ClassModel* val);
signals:
    void toChanged();
private:
    void setToImp(ClassModel* val);
private:
    ClassModel* _to;
private slots:
    void toDeletedSlot();

public:
    friend QDataStream& operator<< (QDataStream& ds, const Links * p);
    friend QDataStream& operator>> (QDataStream& ds, Links * p);

    friend QJsonObject toJson(const Links *p);
    friend void fromJson(const QJsonValue &value, Links *p);

    static void init(int count);
    static void load(QDataStream& ds);
    static void save(QDataStream& ds);
    static void loadFromJson(const QJsonArray &);
    static QJsonArray saveToJson();
    static void createIndex();
    static void clearIndex();
    static void deleteAll();
    static QList<Links*> _ptrs;
    static QHash<Links*, int> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const Links * p);
QDataStream& operator>> (QDataStream& ds, Links * p);

QJsonObject toJson(const Links *p);
void fromJson(const QJsonValue &value, Links *p);

#endif // LINKS_H

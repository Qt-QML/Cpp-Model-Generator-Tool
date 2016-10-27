#ifndef ENUMVALUE_H
#define ENUMVALUE_H

#include "classes.h"

class EnumValue : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE EnumValue(QObject *parent = Q_NULLPTR);
    ~EnumValue();


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

public:
    friend QDataStream& operator<< (QDataStream& ds, const EnumValue * p);
    friend QDataStream& operator>> (QDataStream& ds, EnumValue * p);

    friend QJsonObject toJson(const EnumValue *p);
    friend void fromJson(const QJsonValue &value, EnumValue *p);

    static void init(int count);
    static void load(QDataStream& ds);
    static void save(QDataStream& ds);
    static void loadFromJson(const QJsonArray &);
    static QJsonArray saveToJson();
    static void createIndex();
    static void clearIndex();
    static void deleteAll();
    static QList<EnumValue*> _ptrs;
    static QHash<EnumValue*, int> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const EnumValue * p);
QDataStream& operator>> (QDataStream& ds, EnumValue * p);

QJsonObject toJson(const EnumValue *p);
void fromJson(const QJsonValue &value, EnumValue *p);

#endif // ENUMVALUE_H

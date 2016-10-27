#ifndef CLASSPROP_H
#define CLASSPROP_H

#include "classes.h"

class ClassProp : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE ClassProp(QObject *parent = Q_NULLPTR);
    ~ClassProp();


    // ----[ read ] ----
public:
    Q_PROPERTY(bool read  READ read  WRITE setRead  NOTIFY readChanged  )
    bool read() const { return _read; }
    void setRead(bool val);
signals:
    void readChanged();
private:
    void setReadImp(bool val);
private:
    bool _read;


    // ----[ save ] ----
public:
    Q_PROPERTY(bool save  READ save  WRITE setSave  NOTIFY saveChanged  )
    bool save() const { return _save; }
    void setSave(bool val);
signals:
    void saveChanged();
private:
    void setSaveImp(bool val);
private:
    bool _save;


    // ----[ notify ] ----
public:
    Q_PROPERTY(bool notify  READ notify  WRITE setNotify  NOTIFY notifyChanged  )
    bool notify() const { return _notify; }
    void setNotify(bool val);
signals:
    void notifyChanged();
private:
    void setNotifyImp(bool val);
private:
    bool _notify;


    // ----[ undo ] ----
public:
    Q_PROPERTY(bool undo  READ undo  WRITE setUndo  NOTIFY undoChanged  )
    bool undo() const { return _undo; }
    void setUndo(bool val);
signals:
    void undoChanged();
private:
    void setUndoImp(bool val);
private:
    bool _undo;


    // ----[ validate ] ----
public:
    Q_PROPERTY(QString validate  READ validate  WRITE setValidate  NOTIFY validateChanged  )
    QString validate() const { return _validate; }
    void setValidate(QString val);
signals:
    void validateChanged();
private:
    void setValidateImp(QString val);
private:
    QString _validate;


    // ----[ destruct ] ----
public:
    Q_PROPERTY(QString destruct  READ destruct  WRITE setDestruct  NOTIFY destructChanged  )
    QString destruct() const { return _destruct; }
    void setDestruct(QString val);
signals:
    void destructChanged();
private:
    void setDestructImp(QString val);
private:
    QString _destruct;


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


    // ----[ type ] ----
public:
    Q_PROPERTY(QString type  READ type  WRITE setType  NOTIFY typeChanged  )
    QString type() const { return _type; }
    void setType(QString val);
signals:
    void typeChanged();
private:
    void setTypeImp(QString val);
private:
    QString _type;


    // ----[ subType ] ----
public:
    Q_PROPERTY(QString subType  READ subType  WRITE setSubType  NOTIFY subTypeChanged  )
    QString subType() const { return _subType; }
    void setSubType(QString val);
signals:
    void subTypeChanged();
private:
    void setSubTypeImp(QString val);
private:
    QString _subType;


    // ----[ init ] ----
public:
    Q_PROPERTY(QString init  READ init  WRITE setInit  NOTIFY initChanged  )
    QString init() const { return _init; }
    void setInit(QString val);
signals:
    void initChanged();
private:
    void setInitImp(QString val);
private:
    QString _init;


    // ----[ write ] ----
public:
    Q_PROPERTY(bool write  READ write  WRITE setWrite  NOTIFY writeChanged  )
    bool write() const { return _write; }
    void setWrite(bool val);
signals:
    void writeChanged();
private:
    void setWriteImp(bool val);
private:
    bool _write;


    // ----[ count ] ----
public:
    Q_PROPERTY(qint32 count  READ count  WRITE setCount  NOTIFY countChanged  )
    qint32 count() const { return _count; }
    void setCount(qint32 val);
signals:
    void countChanged();
private:
    void setCountImp(qint32 val);
private:
    qint32 _count;


    // ----[ null ] ----
public:
    Q_PROPERTY(bool null  READ null  WRITE setNull  NOTIFY nullChanged  )
    bool null() const { return _null; }
    void setNull(bool val);
signals:
    void nullChanged();
private:
    void setNullImp(bool val);
private:
    bool _null;


    // ----[ cpos ] ----
public:
    Q_PROPERTY(QPointF cpos  READ cpos  WRITE setCpos  NOTIFY cposChanged  )
    QPointF cpos() const { return _cpos; }
    void setCpos(QPointF val);
signals:
    void cposChanged();
private:
    void setCposImp(QPointF val);
private:
    QPointF _cpos;

public:
    friend QDataStream& operator<< (QDataStream& ds, const ClassProp * p);
    friend QDataStream& operator>> (QDataStream& ds, ClassProp * p);

    friend QJsonObject toJson(const ClassProp *p);
    friend void fromJson(const QJsonValue &value, ClassProp *p);

    static void init(int count);
    static void load(QDataStream& ds);
    static void save(QDataStream& ds);
    static void loadFromJson(const QJsonArray &);
    static QJsonArray saveToJson();
    static void createIndex();
    static void clearIndex();
    static void deleteAll();
    static QList<ClassProp*> _ptrs;
    static QHash<ClassProp*, int> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const ClassProp * p);
QDataStream& operator>> (QDataStream& ds, ClassProp * p);

QJsonObject toJson(const ClassProp *p);
void fromJson(const QJsonValue &value, ClassProp *p);

#endif // CLASSPROP_H

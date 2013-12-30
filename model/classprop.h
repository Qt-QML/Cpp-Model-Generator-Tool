#ifndef CLASSPROP_H
#define CLASSPROP_H

#include "classes.h"

class ClassProp : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE ClassProp(QObject *parent = 0);
    ~ClassProp();


    // ----[ read ] ----
public:
    Q_PROPERTY(bool read  READ read  WRITE setRead  NOTIFY readChanged  )
    bool read() const;
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
    bool save() const;
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
    bool notify() const;
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
    bool undo() const;
    void setUndo(bool val);
signals:
    void undoChanged();
private:
    void setUndoImp(bool val);
private:
    bool _undo;

    // ----[ null ] ----
public:
    Q_PROPERTY(bool null  READ null  WRITE setNull  NOTIFY nullChanged  )
    bool null() const;
    void setNull(bool val);
signals:
    void nullChanged();
private:
    void setNullImp(bool val);
private:
    bool _null;


    // ----[ validate ] ----
public:
    Q_PROPERTY(QString validate  READ validate  WRITE setValidate  NOTIFY validateChanged  )
    QString validate() const;
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
    QString destruct() const;
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
    QString name() const;
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
    QString type() const;
    void setType(QString val);
signals:
    void typeChanged();
private:
    void setTypeImp(QString val);
private:
    QString _type;

    // ----[ type ] ----
public:
    Q_PROPERTY(QString subType  READ subType  WRITE setSubType  NOTIFY subTypeChanged  )
    QString subType() const;
    void setSubType(QString val);
signals:
    void subTypeChanged();
private:
    void setSubTypeImp(QString val);
private:
    QString _subType;

    // ----[ type ] ----
public:
    Q_PROPERTY(int count  READ count  WRITE setCount  NOTIFY countChanged  )
    int count() const;
    void setCount(int val);
signals:
    void countChanged();
private:
    int _count;


    // ----[ init ] ----
public:
    Q_PROPERTY(QString init  READ init  WRITE setInit  NOTIFY initChanged  )
    QString init() const;
    void setInit(QString val);
signals:
    void initChanged();
private:
    void setInitImp(QString val);
private:
    QString _init;

    // ----[ cpos ] ----
public:
    Q_PROPERTY(QPointF cpos  READ cpos  WRITE setCPos  NOTIFY cposChanged  )
    QPointF cpos() const;
    void setCPos(QPointF val);
signals:
    void cposChanged();
private:
    void setCPosImp(QPointF val);
private:
    QPointF _cpos;



    // ----[ write ] ----
public:
    Q_PROPERTY(bool write  READ write  WRITE setWrite  NOTIFY writeChanged  )
    bool write() const;
    void setWrite(bool val);
signals:
    void writeChanged();
private:
    void setWriteImp(bool val);
private:
    bool _write;

private:
    void updateInit();
public:
    QString generateH() const;
    QString generateCPP(const QString &classname) const;

    QString generateSave() const;
    QString generateLoad() const;


    friend QDataStream& operator<< (QDataStream& ds, const ClassProp * p);
    friend QDataStream& operator>> (QDataStream& ds, ClassProp * p);

    static void init(int count);
    static void load(QDataStream& ds);
    static void save(QDataStream& ds);
    static void createIndex();
    static void clearIndex();
    static QList<ClassProp*> _ptrs;
    static QHash<ClassProp*, quint32> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const ClassProp * p);
QDataStream& operator>> (QDataStream& ds, ClassProp * p);
#endif // CLASSPROP_H

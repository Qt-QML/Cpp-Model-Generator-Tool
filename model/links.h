#ifndef LINKS_H
#define LINKS_H

#include "classes.h"

class Links : public QObject
{
	Q_OBJECT
public:
	Q_INVOKABLE Links(QObject *parent = 0);
	~Links();


// ----[ from ] ----
public:
	Q_PROPERTY(ClassProp* from  READ from  WRITE setFrom  NOTIFY fromChanged  )
	ClassProp* from() const;
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
	ClassModel* to() const;
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

	static void init(int count);
	static void load(QDataStream& ds);
	static void save(QDataStream& ds);
	static void createIndex();
	static void clearIndex();
	static QList<Links*> _ptrs;
    static QHash<Links*, quint32> _indexedPtrs;
};
QDataStream& operator<< (QDataStream& ds, const Links * p);
QDataStream& operator>> (QDataStream& ds, Links * p);
#endif // LINKS_H

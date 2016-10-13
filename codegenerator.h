#ifndef CODEGENERATOR_H
#define CODEGENERATOR_H

#include <QObject>

class CodeGenerator : public QObject
{
    Q_OBJECT
public:
    explicit CodeGenerator(QObject *parent = Q_NULLPTR);

signals:

public slots:
    void generateFiles(QObject *modelObject, const QString &folder) const;
};

#endif // CODEGENERATOR_H

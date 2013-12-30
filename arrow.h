#ifndef ARROW_H
#define ARROW_H

#include <QQuickPaintedItem>

class Arrow : public QQuickPaintedItem
{
    Q_OBJECT
public:
    explicit Arrow(QQuickItem *parent = 0);
    
    virtual void paint(QPainter * painter);

signals:
    
public slots:
    
};

#endif // ARROW_H

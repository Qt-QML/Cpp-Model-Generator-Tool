#include "arrow.h"

#include <QDebug>
#include <QPainter>

Arrow::Arrow(QQuickItem *parent) :
    QQuickPaintedItem(parent)
{
    setAntialiasing(true);
}


void Arrow::paint(QPainter * painter)
{

    QPointF from(mapFromItem(parentItem(), QPointF(parentItem()->property("fx").toDouble(), parentItem()->property("fy").toDouble())));
    QPointF to(mapFromItem(parentItem(), QPointF(parentItem()->property("tx").toDouble(), parentItem()->property("ty").toDouble())));

    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QColor(0x30,0x30,0x30));

//    painter->setBrush(QColor(0,0,0,10));
//    painter->drawRect(boundingRect());

//    qreal fy = ((to.y() - from.y())/(to.x() - from.x())) * (60 - from.x()) + from.y();
//    qreal fx = ((to.x() - from.x())/(to.y() - from.y())) * (60 - from.y()) + from.x();

    painter->drawLine(from*0.98 + to*0.02, from*0.02 + to*0.98);

    QPointF end(from*0.02 + to*0.98);


    painter->setPen(Qt::NoPen);
    painter->setBrush(QColor(0,0,0,0x40));
    painter->drawEllipse(end, 4, 4);

}

#ifndef JSONCONVERTERS_H
#define JSONCONVERTERS_H

#include <QColor>
#include <QJsonArray>
#include <QJsonObject>
#include <QPointF>
#include <QRectF>

inline QJsonValue toJson(bool b) { return b; }
inline QJsonValue toJson(int i) { return i; }
inline QJsonValue toJson(double d) { return d; }
inline QJsonValue toJson(const QString &s) { return s; }

inline QJsonValue toJson(const QPointF &point)
{
    QJsonObject object;
    object.insert(QLatin1String("x"), point.x());
    object.insert(QLatin1String("y"), point.y());
    return object;
}

inline QJsonValue toJson(const QRectF &rect)
{
    QJsonObject object;
    object.insert(QLatin1String("x"), rect.x());
    object.insert(QLatin1String("y"), rect.y());
    object.insert(QLatin1String("width"), rect.width());
    object.insert(QLatin1String("height"), rect.height());
    return object;
}

inline QJsonValue toJson(const QColor &color)
{
    return color.name(QColor::HexArgb);
}


inline void fromJson(const QJsonValue &value, bool &b)
{
    b = value.toBool();
}

inline void fromJson(const QJsonValue &value, int &i)
{
    i = value.toInt();
}

inline void fromJson(const QJsonValue &value, double &d)
{
    d = value.toDouble();
}

inline void fromJson(const QJsonValue &value, QString &s)
{
    s = value.toString();
}

inline void fromJson(const QJsonValue &value, QPointF &point)
{
    const QJsonObject object = value.toObject();
    point.setX(object[QLatin1String("x")].toDouble());
    point.setY(object[QLatin1String("y")].toDouble());
}

inline void fromJson(const QJsonValue &value, QRectF &rect)
{
    const QJsonObject object = value.toObject();
    double x, y, width, height;
    fromJson(object[QLatin1String("x")], x);
    fromJson(object[QLatin1String("y")], y);
    fromJson(object[QLatin1String("width")], width);
    fromJson(object[QLatin1String("height")], height);
    rect.setRect(x, y, width, height);
}

inline void fromJson(const QJsonValue &value, QColor &color)
{
    color.setNamedColor(value.toString());
}

#endif // JSONCONVERTERS_H

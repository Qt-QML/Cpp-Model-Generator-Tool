#ifndef CLASSESMODEL_H
#define CLASSESMODEL_H

#include <QObject>
#include <QColor>
#include <QImage>
#include <QVariant>
#include <QPoint>
#include <QPointF>
#include <QList>
#include <QHash>
#include <QSet>
#include <QRectF>

class ClassModel;
class ClassProp;
class Enum;
class EnumValue;
class Links;
class Model;

#include "../model_engine/jsonconverters.h"
#include "../model_engine/objectlist.h"
#include "../model_engine/undoer.h"

#include "classmodel.h"
#include "classprop.h"
#include "model.h"
#include "links.h"
#include "enum.h"
#include "enumvalue.h"

#endif // CLASSESMODEL_H

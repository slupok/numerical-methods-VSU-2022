#ifndef POINTINFO_H
#define POINTINFO_H

#include <QPointF>

struct PointInfo {
  // x y
  QPointF coordinate = {};

  // u v
  QPointF displacement = {};
};

#endif // POINTINFO_H

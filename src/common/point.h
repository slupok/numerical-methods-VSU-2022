#ifndef POINT_H
#define POINT_H

#include <QPointF>

#include "stiffnessutils.h"

struct Point {
  // x y
  QPointF coordinate = {};

  // u v
  QPointF displacement = {};

  StiffnessUtils::Constraints::Type constraintType =
      StiffnessUtils::Constraints::Type::None;
};

#endif // POINT_H

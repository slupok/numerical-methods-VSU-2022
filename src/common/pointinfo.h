#ifndef POINTINFO_H
#define POINTINFO_H

#include <QPointF>

#include "stiffnessutils.h"

struct PointInfo {
  // x y
  QPointF coordinate = {};

  // u v
  QPointF displacement = {};

  StiffnessUtils::Constraints::Type constraintType =
      StiffnessUtils::Constraints::Type::None;
};

#endif // POINTINFO_H

#ifndef FIGURE_H
#define FIGURE_H

#include <QPointF>
#include <QVector>

struct Figure {
  QVector<QPointF> points;
  QVector<int> triangleIndices;
};

#endif // FIGURE_H

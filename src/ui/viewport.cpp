#include "viewport.h"

Viewport::Viewport(QWidget *parent) : QGraphicsView(parent) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setRenderHint(QPainter::Antialiasing);

  m_scene = new QGraphicsScene();
  setScene(m_scene);

  scale(5, 5);
}

void Viewport::setPoints(const QVector<QPointF> &points) { m_points = points; }

void Viewport::setTriangleIndices(const QVector<int> &triangleIndices) {
  m_triangleIndices = triangleIndices;
}

void Viewport::update() {
  m_scene->clear();

  drawTriangles();
  drawPoints();
}

int Viewport::selectedPoint() const { return m_pointSelected; }

void Viewport::mousePressEvent(QMouseEvent *event) {
  const QPointF mousePosition =
      mapToScene(event->localPos().toPoint()) * 1.f / m_scaleFactor;

  for (int pointIndex = 0; pointIndex < m_points.size(); ++pointIndex) {
    QPointF point = m_points[pointIndex];

    if ((mousePosition - point).manhattanLength() <= m_radiusSearch) {
      m_pointSelected = pointIndex;
      emit onPointSelected(m_pointSelected);
      return;
    }
  }

  m_pointSelected = -1;
}

void Viewport::wheelEvent(QWheelEvent *event) {
  if (event->delta() < 0)
    m_currentZoom += 1;
  else
    m_currentZoom -= 1;
}

void Viewport::drawPoints() {
  if (!m_scene)
    return;

  QPen pointPen = QPen(m_pointColor);

  for (int pointIndex = 0; pointIndex < m_points.size(); ++pointIndex) {
    QPointF pointInLocalCoord = m_points[pointIndex] * m_scaleFactor;

    m_scene->addEllipse(pointInLocalCoord.x() - m_pointSize,
                        pointInLocalCoord.y() - m_pointSize, m_pointSize * 2,
                        m_pointSize * 2, pointPen, QBrush(m_pointColor));
  }
}

void Viewport::drawTriangles() {
  if (!m_scene)
    return;

  QPen linePen = QPen(m_lineColor);

  for (int pointIndex = 0; pointIndex < m_triangleIndices.size() - 2;
       pointIndex += 3) {
    const double x1 =
        m_points[m_triangleIndices[pointIndex]].x() * m_scaleFactor;
    const double y1 =
        m_points[m_triangleIndices[pointIndex]].y() * m_scaleFactor;

    const double x2 =
        m_points[m_triangleIndices[pointIndex + 1]].x() * m_scaleFactor;
    const double y2 =
        m_points[m_triangleIndices[pointIndex + 1]].y() * m_scaleFactor;

    const double x3 =
        m_points[m_triangleIndices[pointIndex + 2]].x() * m_scaleFactor;
    const double y3 =
        m_points[m_triangleIndices[pointIndex + 2]].y() * m_scaleFactor;

    m_scene->addLine(x1, y1, x2, y2, linePen);
    m_scene->addLine(x2, y2, x3, y3, linePen);
    m_scene->addLine(x3, y3, x1, y1, linePen);
  }
}

#include "viewport.h"

#include <QPolygonF>

Viewport::Viewport(QWidget *parent) : QGraphicsView(parent) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setRenderHint(QPainter::Antialiasing);

  setScene(&mScene);
}

Viewport::~Viewport() {}

void Viewport::update() {
  mScene.clear();

  drawTriangles();
  drawPoints();

  QGraphicsView::update();
}

void Viewport::drawPoints() {
  QPen pointPen(mPointColor);
  QBrush pointBrush(mPointColor);

  for (int i = 0; i < mFigure.points.size(); ++i) {
    QPointF pointInLocalCoord = mFigure.points[i] * mScaleFactor;

    mScene.addEllipse(pointInLocalCoord.x() - mPointSize,
                      pointInLocalCoord.y() - mPointSize, mPointSize * 2,
                      mPointSize * 2, pointPen, pointBrush);
  }

  if (mPointSelected != -1) {
    QPen selectedPointPen(mSelectedPointColor);
    QBrush selectedPointBrush(mSelectedPointColor);

    QPointF pointInLocalCoord = mFigure.points[mPointSelected] * mScaleFactor;

    mScene.addEllipse(pointInLocalCoord.x() - mPointSize,
                      pointInLocalCoord.y() - mPointSize, mPointSize * 2,
                      mPointSize * 2, selectedPointPen, selectedPointBrush);
  }
}

void Viewport::drawTriangles() {
  QPen linePen = QPen(mLineColor);

  for (int pointIndex = 0; pointIndex < mFigure.triangleIndices.size() - 2;
       pointIndex += 3) {
    const auto a =
        mFigure.points[mFigure.triangleIndices[pointIndex]] * mScaleFactor;

    const auto b =
        mFigure.points[mFigure.triangleIndices[pointIndex + 1]] * mScaleFactor;

    const auto c =
        mFigure.points[mFigure.triangleIndices[pointIndex + 2]] * mScaleFactor;

    mScene.addLine(QLineF(a, b), linePen);
    mScene.addLine(QLineF(b, c), linePen);
    mScene.addLine(QLineF(c, a), linePen);
  }
}

const Figure &Viewport::getFigure() const { return mFigure; }

void Viewport::setFigure(const Figure &Figure) { mFigure = Figure; }

void Viewport::fitInScreen() {
  this->fitInView(mScene.sceneRect(), Qt::KeepAspectRatio);
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *event) {}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  if (mPointSelected == invalidIndex) {
    return;
  }

  const QPointF mousePosition =
      mapToScene(event->localPos().toPoint()) * 1.f / mScaleFactor;

  mFigure.points[mPointSelected] = mousePosition;

  pointCoordinateChanged(mFigure.points[mPointSelected]);

  this->QGraphicsView::mouseMoveEvent(event);

  this->update();
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  const QPointF mousePosition =
      mapToScene(event->localPos().toPoint()) * 1.f / mScaleFactor;

  for (int pointIndex = 0; pointIndex < mFigure.points.size(); ++pointIndex) {
    QPointF point = mFigure.points[pointIndex];

    if ((mousePosition - point).manhattanLength() <= mRadiusSearch) {
      mPointSelected = pointIndex;
      emit onPointSelected(mPointSelected);
      return;
    }
  }

  mPointSelected = -1;
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) {}

void Viewport::wheelEvent(QWheelEvent *event) {}

const QColor &Viewport::getPointColor() const { return mPointColor; }

void Viewport::setPointColor(const QColor &PointColor) {
  mPointColor = PointColor;
}

const QColor &Viewport::getSelectedPointColor() const {
  return mSelectedPointColor;
}

void Viewport::setSelectedPointColor(const QColor &SelectedPointColor) {
  mSelectedPointColor = SelectedPointColor;
}

const QColor &Viewport::getLineColor() const { return mLineColor; }

void Viewport::setLineColor(const QColor &LineColor) { mLineColor = LineColor; }

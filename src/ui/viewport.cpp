#include "viewport.h"

#include <QPolygonF>

Viewport::Viewport(QWidget *parent) : QGraphicsView(parent) {
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setRenderHint(QPainter::Antialiasing);

  setScene(&mScene);

  scale(1, -1);
}

Viewport::~Viewport() {}

void Viewport::update() {
  mScene.clear();

  drawAxes();

  if (mShowFigure) {
    drawFigure(mFigure);
  }

  if (mShowSolvedFigure) {
    drawFigure(mSolvedFigure);
  }

  QGraphicsView::update();
}

void Viewport::drawPoints(const Figure &figure) {
  QPen pointPen(mPointColor);
  QBrush pointBrush(mPointColor);

  for (int i = 0; i < figure.points.size(); ++i) {
    QPointF pointInLocalCoord = figure.points[i] * mScaleFactor;

    mScene.addEllipse(pointInLocalCoord.x() - mPointSize,
                      pointInLocalCoord.y() - mPointSize, mPointSize * 2,
                      mPointSize * 2, pointPen, pointBrush);
  }

  if (mPointSelected != -1) {
    QPen selectedPointPen(mSelectedPointColor);
    QBrush selectedPointBrush(mSelectedPointColor);

    QPointF pointInLocalCoord = figure.points[mPointSelected] * mScaleFactor;

    mScene.addEllipse(pointInLocalCoord.x() - mPointSize,
                      pointInLocalCoord.y() - mPointSize, mPointSize * 2,
                      mPointSize * 2, selectedPointPen, selectedPointBrush);
  }
}

void Viewport::drawTriangles(const Figure &figure) {
  QPen linePen = QPen(mLineColor);

  for (int pointIndex = 0; pointIndex < figure.triangleIndices.size() - 2;
       pointIndex += 3) {
    const auto a =
        figure.points[figure.triangleIndices[pointIndex]] * mScaleFactor;

    const auto b =
        figure.points[figure.triangleIndices[pointIndex + 1]] * mScaleFactor;

    const auto c =
        figure.points[figure.triangleIndices[pointIndex + 2]] * mScaleFactor;

    mScene.addLine(QLineF(a, b), linePen);
    mScene.addLine(QLineF(b, c), linePen);
    mScene.addLine(QLineF(c, a), linePen);
  }
}

void Viewport::drawAxes() {}

QPointF Viewport::toScene(const QPointF &screen) {
  return mapToScene(screen.toPoint()) * 1.f / mScaleFactor;
}

bool Viewport::getShowSolvedFigure() const { return mShowSolvedFigure; }

void Viewport::setShowSolvedFigure(bool ShowSolvedFigure) {
  mShowSolvedFigure = ShowSolvedFigure;
  this->update();
}

void Viewport::clear() { mScene.clear(); }

bool Viewport::getShowFigure() const { return mShowFigure; }

void Viewport::setShowFigure(bool ShowFigure) {
  mShowFigure = ShowFigure;
  this->update();
}

const Figure &Viewport::getSolvedFigure() const { return mSolvedFigure; }

void Viewport::setSolvedFigure(const Figure &SolvedFigure) {
  mSolvedFigure = SolvedFigure;
}

const Figure &Viewport::getFigure() const { return mFigure; }

void Viewport::setFigure(const Figure &Figure) { mFigure = Figure; }

void Viewport::fitInScreen() {
  this->fitInView(mScene.itemsBoundingRect(), Qt::KeepAspectRatio);
}

void Viewport::mouseDoubleClickEvent(QMouseEvent *event) {}

void Viewport::mouseMoveEvent(QMouseEvent *event) {
  if (mPointSelected == invalidIndex) {
    return;
  }

  const QPointF mousePosition = toScene(event->localPos());

  mFigure.points[mPointSelected] = mousePosition;

  pointCoordinateChanged(mFigure.points[mPointSelected]);

  this->QGraphicsView::mouseMoveEvent(event);

  this->update();
}

void Viewport::mousePressEvent(QMouseEvent *event) {
  const QPointF mousePosition = toScene(event->localPos());

  for (int pointIndex = 0; pointIndex < mFigure.points.size(); ++pointIndex) {
    QPointF point = mFigure.points[pointIndex];

    if ((mousePosition - point).manhattanLength() <= mRadiusSearch) {
      mPointSelected = pointIndex;
      emit onPointSelected(mPointSelected);
      this->update();
      return;
    }
  }

  mPointSelected = -1;

  emit onPointDeselect();
  this->update();
}

void Viewport::mouseReleaseEvent(QMouseEvent *event) {}

void Viewport::wheelEvent(QWheelEvent *event) {
  if (event->delta() > 0) {
    this->scale(1.1, 1.1);
  } else {
    this->scale(0.9, 0.9);
  }

  this->update();
}

void Viewport::drawFigure(const Figure &figure) {
  if (figure.points.isEmpty()) {
    return;
  }

  drawTriangles(figure);
  drawPoints(figure);
}

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

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QDebug>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QMouseEvent>
#include <QWheelEvent>

#include "common/figure.h"

class Viewport : public QGraphicsView {
  Q_OBJECT
public:
  Viewport(QWidget *parent = nullptr);
  ~Viewport();

  void update();

  const QColor &getLineColor() const;
  void setLineColor(const QColor &LineColor);

  const QColor &getSelectedPointColor() const;
  void setSelectedPointColor(const QColor &SelectedPointColor);

  const QColor &getPointColor() const;
  void setPointColor(const QColor &PointColor);

  const Figure &getFigure() const;
  void setFigure(const Figure &Figure);

  const Figure &getSolvedFigure() const;
  void setSolvedFigure(const Figure &SolvedFigure);

  bool getShowFigure() const;
  void setShowFigure(bool ShowFigure);

  bool getShowSolvedFigure() const;
  void setShowSolvedFigure(bool ShowSolvedFigure);

  void clear();

public slots:
  void fitInScreen();

signals:
  void onPointSelected(const int pointIndex);
  void onPointDeselect();
  void pointCoordinateChanged(const QPointF &point);

protected:
  virtual void mouseDoubleClickEvent(QMouseEvent *event) override;
  virtual void mouseMoveEvent(QMouseEvent *event) override;
  virtual void mousePressEvent(QMouseEvent *event) override;
  virtual void mouseReleaseEvent(QMouseEvent *event) override;
  virtual void wheelEvent(QWheelEvent *event) override;

private:
  void drawFigure(const Figure &figure);
  void drawPoints(const Figure &figure);
  void drawTriangles(const Figure &figure);
  void drawAxes();

  QPointF toScene(const QPointF &screen);

  QGraphicsScene mScene;

  static constexpr const int invalidIndex = -1;

  const float mRadiusSearch = 0.1f;
  int mPointSelected = invalidIndex;

  const int mPointSize = 2;
  const float mScaleFactor = 30.0f;

  QColor mPointColor = Qt::green;
  QColor mSelectedPointColor = Qt::red;
  QColor mLineColor = Qt::blue;
  QColor mAxisColor = Qt::black;

  Figure mFigure;
  Figure mSolvedFigure;

  bool mShowFigure = true;
  bool mShowSolvedFigure = false;
};

#endif // VIEWPORT_H

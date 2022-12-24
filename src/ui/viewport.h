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

public slots:
  void fitInScreen();

signals:
  void onPointSelected(const int pointIndex);

protected:
  void mousePressEvent(QMouseEvent *event) override;
  void wheelEvent(QWheelEvent *event) override;

private:
  void drawPoints();
  void drawTriangles();

  QGraphicsScene mScene;

  const float mRadiusSearch = 0.1f;
  int mPointSelected = -1;

  const int mPointSize = 2;
  const float mScaleFactor = 30.0f;

  QColor mPointColor = Qt::green;
  QColor mSelectedPointColor = Qt::red;
  QColor mLineColor = Qt::blue;

  Figure mFigure;
};

#endif // VIEWPORT_H

#ifndef POINTEDITOR_H
#define POINTEDITOR_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "common/point.h"

class PointEditor : public QWidget {
  Q_OBJECT
public:
  explicit PointEditor(QWidget *parent = nullptr,
                       Qt::WindowFlags f = Qt::WindowFlags());

  Point getPoint() const;
  void setPoint(const Point &point);

signals:
  void pointInfoChanged(const Point &point);

public slots:
  void disableUI();
  void enableUI();

private:
  void createLayout();
  void createConnections();

  Point mPoint;

  // Coordinate
  QDoubleSpinBox *mXCoordinateSpinBox = nullptr;
  QDoubleSpinBox *mYCoordinateSpinBox = nullptr;

  // Displacement
  QDoubleSpinBox *mUDisplacementSpinBox = nullptr;
  QDoubleSpinBox *mVDisplacementSpinBox = nullptr;

  // Constraint
  QComboBox *mConstraints = nullptr;
};

#endif // POINTEDITOR_H

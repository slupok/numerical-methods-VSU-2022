#ifndef POINTEDITOR_H
#define POINTEDITOR_H

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QPushButton>
#include <QWidget>

#include "common/pointinfo.h"

class PointEditor : public QWidget {
  Q_OBJECT
public:
  explicit PointEditor(QWidget *parent = nullptr,
                       Qt::WindowFlags f = Qt::WindowFlags());

  PointInfo getPointInfo() const;
  void setPointInfo(PointInfo PointInfo);

signals:
  void pointChanged(const PointInfo &info);

public slots:
  void disableUI();
  void enableUI();

private:
  void createLayout();
  void createConnections();

  PointInfo mPointInfo;

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

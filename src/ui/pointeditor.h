#ifndef POINTEDITOR_H
#define POINTEDITOR_H

#include <QComboBox>
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

public slots:
  void disableUI();
  void enableUI();

private:
  void createLayout();

  PointInfo mPointInfo;

  // Constraints settings
  QComboBox *m_constraintsByAxis = nullptr;
  //

  // Material settings
  QLineEdit *m_ETextEdit = nullptr;
  QLineEdit *m_vTextEdit = nullptr;
  //

  // Comptue buutton
  QPushButton *m_computeButton = nullptr;
  //

  // Displacment settings
  QLineEdit *m_uDisplacmentTextEdit = nullptr;
  QLineEdit *m_vDisplacmentTextEdit = nullptr;
  //

  // Point coord settings
  QLineEdit *m_xCoordTextEdit = nullptr;
  QLineEdit *m_yCoordTextEdit = nullptr;
  //

  QPushButton *m_applySettings = nullptr;
};

#endif // POINTEDITOR_H

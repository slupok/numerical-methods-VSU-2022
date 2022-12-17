#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loader/loaderthread.h"

#include "stiffnessutils.h"
#include "viewport.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

private slots:
  void pointSelected(const int pointIndex);
  void constraintsIndexChanged(int index);

  void uDisplacmentChanged(const QString &u);
  void vDisplacmentChanged(const QString &v);

  void applySettings();

  void computeDisplacment();

  void open();

  void loaded();
  void failed();

private:
  void createLayout();
  void createMenuBar();

  void disableUI();
  void enableUI();

  Viewport *m_viewport = nullptr;

  // Constraints settings
  QLabel *m_constraintsByAxisLabel = nullptr;
  QComboBox *m_constraintsByAxis = nullptr;
  //

  QLabel *m_pointIndex = nullptr;

  // Material settings
  QLabel *m_ELabel = nullptr;
  QLineEdit *m_ETextEdit = nullptr;

  QLabel *m_vLabel = nullptr;
  QLineEdit *m_vTextEdit = nullptr;
  //

  // Comptue buutton
  QPushButton *m_computeButton = nullptr;
  //

  // Displacment settings
  QLabel *m_uDisplacmentLabel = nullptr;
  QLineEdit *m_uDisplacmentTextEdit = nullptr;

  QLabel *m_vDisplacmentLabel = nullptr;
  QLineEdit *m_vDisplacmentTextEdit = nullptr;
  //

  // Point coord settings
  QLabel *m_xCoordLabel = nullptr;
  QLineEdit *m_xCoordTextEdit = nullptr;

  QLabel *m_yCoordLabel = nullptr;
  QLineEdit *m_yCoordTextEdit = nullptr;
  //

  QPushButton *m_applySettings = nullptr;

  // Datas
  QVector<QPointF> m_points;
  QVector<int> m_indices;

  QVector<StiffnessUtils::Displacment> m_displacments;
  QVector<StiffnessUtils::Constraints> m_constraints;

  int m_selectedPointIndex = -1;

  float m_u = 0.0f;
  float m_v = 0.0f;
  StiffnessUtils::Constraints::Type m_type =
      StiffnessUtils::Constraints::Type::None;

  float m_E = 1.0f;
  float m_nu = 0.3f;
  //

  LoaderThread m_loader;
};
#endif // MAINWINDOW_H

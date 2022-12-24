#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "loader/loaderthread.h"

#include "materialeditor.h"
#include "pointeditor.h"
#include "viewport.h"

#include "stiffnessutils.h"

#include "common/figure.h"

#include <DockManager.h>

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
  void pointInfoChanged(const Point &point);

  void computeDisplacment();

  // File
  void open();
  void loaded();
  void failed();

private:
  void createLayout();
  void createConnections();

  void updateViewport();

  Viewport *mViewport = nullptr;
  PointEditor *mPointEditor = nullptr;
  MaterialEditor *mMaterialEditor = nullptr;

  LoaderThread mLoader;

  // Data
  Figure mFigure;

  QVector<StiffnessUtils::Displacment> m_displacments;
  QVector<StiffnessUtils::Constraints> m_constraints;

  int m_selectedPointIndex = -1;

  ads::CDockManager *mDockManager;
};
#endif // MAINWINDOW_H

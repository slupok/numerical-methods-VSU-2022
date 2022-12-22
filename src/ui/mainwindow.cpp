#include "mainwindow.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>

#include <iostream>
#include <vector>

#include <DockAreaWidget.h>

#include "common/pointinfo.h"
#include "triangulationutils.h"
#include "viewport.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->createLayout();
  this->createConnections();

  mPointEditor->disableUI();
}

MainWindow::~MainWindow() { mDockManager->deleteLater(); }

void MainWindow::createLayout() {
  // menu

  QMenuBar *menuBar = this->menuBar();

  {
    QMenu *fileMenu = menuBar->addMenu("File");

    QAction *openAction = fileMenu->addAction("Open");
    openAction->setShortcut(Qt::CTRL | Qt::Key_O);

    QObject::connect(openAction, &QAction::triggered, this, &MainWindow::open);
  }

  {
    QMenu *solutionMenu = menuBar->addMenu("Solution");

    QAction *solveAction = solutionMenu->addAction("Solve");
    solveAction->setShortcut(Qt::CTRL | Qt::Key_C);

    QObject::connect(solveAction, &QAction::triggered, this,
                     &MainWindow::computeDisplacment);
  }

  QMenu *layoutMenu = menuBar->addMenu("Layout");

  // layout

  mDockManager = new ads::CDockManager(this);

  mViewport = new Viewport();
  ads::CDockWidget *viewportDock = new ads::CDockWidget("Viewport");
  viewportDock->setWidget(mViewport);
  ads::CDockAreaWidget *viewportDockArea =
      mDockManager->setCentralWidget(viewportDock);
  viewportDockArea->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

  mMaterialEditor = new MaterialEditor();
  ads::CDockWidget *materialEditorDock =
      new ads::CDockWidget("Material Editor");
  materialEditorDock->setWidget(mMaterialEditor);
  materialEditorDock->setMinimumSizeHintMode(
      ads::CDockWidget::MinimumSizeHintFromContent);
  materialEditorDock->setMinimumSize(200, 400);
  materialEditorDock->resize(200, 400);
  ads::CDockAreaWidget *materialEditorDockArea =
      mDockManager->addDockWidget(ads::RightDockWidgetArea, materialEditorDock);
  layoutMenu->addAction(materialEditorDock->toggleViewAction());

  mPointEditor = new PointEditor();
  ads::CDockWidget *pointEditorDock = new ads::CDockWidget("Point Editor");
  pointEditorDock->setWidget(mPointEditor);
  pointEditorDock->setMinimumSizeHintMode(
      ads::CDockWidget::MinimumSizeHintFromContent);
  pointEditorDock->setMinimumSize(200, 400);
  pointEditorDock->resize(200, 400);
  mDockManager->addDockWidget(ads::BottomDockWidgetArea, pointEditorDock,
                              materialEditorDockArea);
  layoutMenu->addAction(pointEditorDock->toggleViewAction());
}

void MainWindow::createConnections() {
  QObject::connect(&mLoader, &LoaderThread::failed, this, &MainWindow::failed);
  QObject::connect(&mLoader, &LoaderThread::loaded, this, &MainWindow::loaded);

  QObject::connect(mPointEditor, &PointEditor::pointInfoChanged, this,
                   &MainWindow::pointInfoChanged);

  QObject::connect(mViewport, &Viewport::onPointSelected, this,
                   &MainWindow::pointSelected);
}

void MainWindow::updateViewport() {
  mViewport->setPoints(m_points);
  mViewport->setTriangleIndices(m_indices);
  mViewport->update();
}

void MainWindow::pointSelected(const int pointIndex) {
  qDebug() << pointIndex;

  if (pointIndex == -1) {
    mPointEditor->disableUI();
    return;
  }

  mPointEditor->enableUI();

  m_selectedPointIndex = pointIndex;

  const double x = m_points[pointIndex].x();
  const double y = m_points[pointIndex].y();

  PointInfo info;
  info.coordinate.setX(x);
  info.coordinate.setY(y);

  for (int displacmentIndex = 0; displacmentIndex < m_displacments.size();
       ++displacmentIndex) {
    if (m_displacments[displacmentIndex].pointIndex == m_selectedPointIndex) {
      const float u = m_displacments[displacmentIndex].u;
      const float v = m_displacments[displacmentIndex].v;

      info.displacement.setX(u);
      info.displacement.setY(v);

      break;
    }
  }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex) {
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      StiffnessUtils::Constraints::Type constraintsType =
          m_constraints[constraintsIndex].type;
      info.constraintType = constraintsType;

      break;
    }
  }

  mPointEditor->setPointInfo(info);
}

void MainWindow::pointInfoChanged(const PointInfo &info) {
  if (m_selectedPointIndex == -1) {
    return;
  }

  m_points[m_selectedPointIndex] = info.coordinate;

  bool displacmentFound = false;
  bool constraintsFound = false;

  for (int i = 0; i < m_displacments.size(); ++i) {
    if (m_displacments[i].pointIndex == m_selectedPointIndex) {
      m_displacments[i].u = static_cast<float>(info.displacement.x());
      m_displacments[i].v = static_cast<float>(info.displacement.y());

      displacmentFound = true;

      break;
    }
  }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex) {
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      m_constraints[constraintsIndex].type = info.constraintType;

      constraintsFound = true;

      break;
    }
  }

  if (!displacmentFound) {
    StiffnessUtils::Displacment displacment;
    displacment.u = static_cast<float>(info.displacement.x());
    displacment.v = static_cast<float>(info.displacement.y());
    displacment.pointIndex = m_selectedPointIndex;
    m_displacments.push_back(displacment);
  }

  if (!constraintsFound) {
    StiffnessUtils::Constraints constraints;
    constraints.type = mPointEditor->getPointInfo().constraintType;
    constraints.pointIndex = m_selectedPointIndex;
    m_constraints.push_back(constraints);
  }

  this->updateViewport();
}

void MainWindow::computeDisplacment() {
  const auto info = mMaterialEditor->getMaterialInfo();

  StiffnessUtils::compute(info.e, info.v, m_points, m_indices, m_displacments,
                          m_constraints);

  this->updateViewport();
}

////////////////////////////////////////////////////////////////////////////////
/// File
////////////////////////////////////////////////////////////////////////////////

void MainWindow::open() {
  if (mLoader.isRunning()) {
    QMessageBox::warning(this, "warning!",
                         "Unable to load new file, while loading: " +
                             mLoader.getPath());
    return;
  }

  const QString file = QFileDialog::getOpenFileName(this);

  if (file.isEmpty()) {
    return;
  }

  mLoader.setPath(file);
  mLoader.load();
}

void MainWindow::loaded() {
  m_points = mLoader.getVertices();

  std::pair<QVector<int>, QVector<QPointF>> indicesPoints =
      TriangulationUtils::triangulationPolygon(m_points, 2);
  m_indices = indicesPoints.first;
  m_points = indicesPoints.second;

  this->updateViewport();
}

void MainWindow::failed() {
  QMessageBox::warning(this, "warning!",
                       "Unable to load file: " + mLoader.getPath());
}

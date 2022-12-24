#include "mainwindow.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>

#include <iostream>
#include <vector>

#include <DockAreaWidget.h>

#include "common/point.h"
#include "triangulationutils.h"
#include "viewport.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->createLayout();
  this->createConnections();

  mPointEditor->disableUI();
}

MainWindow::~MainWindow() { mDockManager->deleteLater(); }

void MainWindow::createLayout() {
  mViewport = new Viewport();
  mMaterialEditor = new MaterialEditor();
  mPointEditor = new PointEditor();

  // menu
  QMenuBar *menuBar = this->menuBar();

  {
    QMenu *fileMenu = menuBar->addMenu("File");

    QAction *openAction = fileMenu->addAction("Open");
    openAction->setShortcut(Qt::CTRL | Qt::Key_O);

    QObject::connect(openAction, &QAction::triggered, this, &MainWindow::open);
  }

  {
    QMenu *viewMenu = menuBar->addMenu("View");

    QAction *fitInScreenAction = viewMenu->addAction("Fit in screen");
    fitInScreenAction->setShortcut(Qt::Key_F);

    QObject::connect(fitInScreenAction, &QAction::triggered, mViewport,
                     &Viewport::fitInScreen);
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
  ads::CDockWidget *viewportDock = new ads::CDockWidget("Viewport");
  viewportDock->setWidget(mViewport);
  ads::CDockAreaWidget *viewportDockArea =
      mDockManager->setCentralWidget(viewportDock);
  viewportDockArea->setAllowedAreas(ads::DockWidgetArea::OuterDockAreas);

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

  QObject::connect(mPointEditor, &PointEditor::pointChanged, this,
                   &MainWindow::pointInfoChanged);

  QObject::connect(mViewport, &Viewport::onPointSelected, this,
                   &MainWindow::pointSelected);

  QObject::connect(mViewport, &Viewport::onPointDeselect, this,
                   [this]() { this->mPointEditor->disableUI(); });

  QObject::connect(mViewport, &Viewport::pointCoordinateChanged, this,
                   [this](const QPointF &coordinate) {
                     auto point = this->mPointEditor->getPoint();

                     point.coordinate = coordinate;

                     this->mPointEditor->setPoint(point);
                   });
}

void MainWindow::updateViewport() { mViewport->update(); }

void MainWindow::pointSelected(const int pointIndex) {
  qDebug() << pointIndex;

  if (pointIndex == -1) {
    mPointEditor->disableUI();
    return;
  }

  mPointEditor->enableUI();

  m_selectedPointIndex = pointIndex;

  const auto figure = mViewport->getFigure();

  const double x = figure.points[pointIndex].x();
  const double y = figure.points[pointIndex].y();

  Point point;
  point.coordinate.setX(x);
  point.coordinate.setY(y);

  for (int displacmentIndex = 0; displacmentIndex < m_displacments.size();
       ++displacmentIndex) {
    if (m_displacments[displacmentIndex].pointIndex == m_selectedPointIndex) {
      const float u = m_displacments[displacmentIndex].u;
      const float v = m_displacments[displacmentIndex].v;

      point.displacement.setX(u);
      point.displacement.setY(v);

      break;
    }
  }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex) {
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      StiffnessUtils::Constraints::Type constraintsType =
          m_constraints[constraintsIndex].type;
      point.constraintType = constraintsType;

      break;
    }
  }

  mPointEditor->setPoint(point);
}

void MainWindow::pointInfoChanged(const Point &point) {
  if (m_selectedPointIndex == -1) {
    return;
  }

  auto figure = mViewport->getFigure();

  figure.points[m_selectedPointIndex] = point.coordinate;

  bool displacmentFound = false;
  bool constraintsFound = false;

  for (int i = 0; i < m_displacments.size(); ++i) {
    if (m_displacments[i].pointIndex == m_selectedPointIndex) {
      m_displacments[i].u = static_cast<float>(point.displacement.x());
      m_displacments[i].v = static_cast<float>(point.displacement.y());

      displacmentFound = true;

      break;
    }
  }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex) {
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      m_constraints[constraintsIndex].type = point.constraintType;

      constraintsFound = true;

      break;
    }
  }

  if (!displacmentFound) {
    StiffnessUtils::Displacment displacment;
    displacment.u = static_cast<float>(point.displacement.x());
    displacment.v = static_cast<float>(point.displacement.y());
    displacment.pointIndex = m_selectedPointIndex;
    m_displacments.push_back(displacment);
  }

  if (!constraintsFound) {
    StiffnessUtils::Constraints constraints;
    constraints.type = mPointEditor->getPoint().constraintType;
    constraints.pointIndex = m_selectedPointIndex;
    m_constraints.push_back(constraints);
  }

  mViewport->setFigure(figure);
  this->updateViewport();
}

void MainWindow::computeDisplacment() {
  const auto info = mMaterialEditor->getMaterialInfo();

  auto figure = mViewport->getFigure();

  StiffnessUtils::compute(info.e, info.v, figure.points, figure.triangleIndices,
                          m_displacments, m_constraints);

  mViewport->setFigure(figure);
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
  QVector<QPointF> points = mLoader.getVertices();

  std::pair<QVector<int>, QVector<QPointF>> indicesPoints =
      TriangulationUtils::triangulationPolygon(points, 2);

  Figure figure;
  figure.triangleIndices = indicesPoints.first;
  figure.points = indicesPoints.second;

  mViewport->setFigure(figure);
  this->updateViewport();
}

void MainWindow::failed() {
  QMessageBox::warning(this, "warning!",
                       "Unable to load file: " + mLoader.getPath());
}

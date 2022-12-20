#include "mainwindow.h"

#include <QFileDialog>
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMessageBox>

#include <iostream>
#include <vector>

#include "common/pointinfo.h"
#include "triangulationutils.h"
#include "viewport.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->createLayout();
  this->createMenuBar();
  this->createConnections();

  mPointEditor->disableUI();
}

MainWindow::~MainWindow() {}

void MainWindow::createLayout() {
  mViewport = new Viewport(this);

  QVBoxLayout *editorsLayout = new QVBoxLayout();
  editorsLayout->setAlignment(Qt::AlignVCenter | Qt::AlignRight);
  {
    mMaterialEditor = new MaterialEditor(this);
    editorsLayout->addWidget(mMaterialEditor, 0,
                             Qt::AlignVCenter | Qt::AlignRight);

    mPointEditor = new PointEditor(this);
    editorsLayout->addWidget(mPointEditor, 0,
                             Qt::AlignVCenter | Qt::AlignRight);

    mComputeButton = new QPushButton("Compute");
    editorsLayout->addWidget(mComputeButton, 0, Qt::AlignCenter);
  }

  QHBoxLayout *viewportControlLayout = new QHBoxLayout;
  viewportControlLayout->addWidget(mViewport, 0);
  viewportControlLayout->addLayout(editorsLayout, 0);

  viewportControlLayout->setAlignment(Qt::AlignCenter);

  QWidget *centralWidget = new QWidget();
  centralWidget->setLayout(viewportControlLayout);

  setCentralWidget(centralWidget);
}

void MainWindow::createMenuBar() {
  QMenuBar *menuBar = this->menuBar();

  QMenu *fileMenu = menuBar->addMenu("File");

  QAction *openAction = fileMenu->addAction("Open");

  QObject::connect(openAction, &QAction::triggered, this, &MainWindow::open);
}

void MainWindow::createConnections() {
  QObject::connect(&mLoader, &LoaderThread::failed, this, &MainWindow::failed);
  QObject::connect(&mLoader, &LoaderThread::loaded, this, &MainWindow::loaded);

  QObject::connect(mPointEditor, &PointEditor::pointInfoChanged, this,
                   &MainWindow::pointInfoChanged);

  QObject::connect(mViewport, &Viewport::onPointSelected, this,
                   &MainWindow::pointSelected);

  QObject::connect(mComputeButton, &QPushButton::pressed, this,
                   &MainWindow::computeDisplacment);
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

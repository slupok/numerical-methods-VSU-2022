#include "mainwindow.h"

#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <Qt>

#include <iostream>
#include <vector>

#include "common/pointinfo.h"
#include "triangulationutils.h"
#include "viewport.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  this->createLayout();
  this->createMenuBar();

  QObject::connect(&m_loader, &LoaderThread::failed, this, &MainWindow::failed);
  QObject::connect(&m_loader, &LoaderThread::loaded, this, &MainWindow::loaded);
}

MainWindow::~MainWindow() {}

void MainWindow::createLayout() {
  m_viewport = new Viewport(this);
  m_point_editor = new PointEditor(this);

  //
  m_ETextEdit = new QLineEdit;
  m_ETextEdit->setText("1");
  QHBoxLayout *materialELayout = new QHBoxLayout;
  materialELayout->addWidget(new QLabel("E: "));
  materialELayout->addWidget(m_ETextEdit);

  m_vTextEdit = new QLineEdit;
  m_vTextEdit->setText("0.3");
  QHBoxLayout *materialVLayout = new QHBoxLayout;
  materialVLayout->addWidget(new QLabel("v: "));
  materialVLayout->addWidget(m_vTextEdit);

  m_xCoordTextEdit = new QLineEdit;
  QHBoxLayout *coordXLayout = new QHBoxLayout;
  coordXLayout->addWidget(new QLabel("x: "));
  coordXLayout->addWidget(m_xCoordTextEdit);

  m_yCoordTextEdit = new QLineEdit;
  QHBoxLayout *coordYLayout = new QHBoxLayout;
  coordYLayout->addWidget(new QLabel("y: "));
  coordYLayout->addWidget(m_yCoordTextEdit);

  m_uDisplacmentTextEdit = new QLineEdit;
  m_uDisplacmentTextEdit->setText("0");
  QHBoxLayout *uDisplacmentLayout = new QHBoxLayout;
  uDisplacmentLayout->addWidget(new QLabel("u: "));
  uDisplacmentLayout->addWidget(m_uDisplacmentTextEdit);

  m_vDisplacmentTextEdit = new QLineEdit;
  m_vDisplacmentTextEdit->setText("0");
  QHBoxLayout *vDisplacmentLayout = new QHBoxLayout;
  vDisplacmentLayout->addWidget(new QLabel("v: "));
  vDisplacmentLayout->addWidget(m_vDisplacmentTextEdit);

  m_applySettings = new QPushButton("Apply");
  m_computeButton = new QPushButton("Compute");

  m_constraintsByAxis = new QComboBox;
  m_constraintsByAxis->addItems(QStringList() << "None"
                                              << "X"
                                              << "Y"
                                              << "XY");
  QHBoxLayout *constraintsByAxisLayout = new QHBoxLayout;
  constraintsByAxisLayout->addWidget(new QLabel("Constraints: "));
  constraintsByAxisLayout->addWidget(m_constraintsByAxis);

  //

  QVBoxLayout *controlsLayout = new QVBoxLayout;
  controlsLayout->addLayout(materialELayout);
  controlsLayout->addLayout(materialVLayout);
  controlsLayout->addLayout(coordXLayout);
  controlsLayout->addLayout(coordYLayout);
  controlsLayout->addLayout(uDisplacmentLayout);
  controlsLayout->addLayout(vDisplacmentLayout);
  controlsLayout->addLayout(constraintsByAxisLayout);
  controlsLayout->addWidget(m_applySettings);
  controlsLayout->addWidget(m_computeButton);
  controlsLayout->setAlignment(Qt::AlignCenter);

  QHBoxLayout *viewportControlLayout = new QHBoxLayout;
  viewportControlLayout->addWidget(m_viewport, 0);
  viewportControlLayout->addWidget(m_point_editor, 0);
  viewportControlLayout->addLayout(controlsLayout, 0);

  viewportControlLayout->setAlignment(Qt::AlignCenter);

  QWidget *centralWidget = new QWidget();
  centralWidget->setLayout(viewportControlLayout);

  setCentralWidget(centralWidget);

  disableUI();

  connect(m_viewport, &Viewport::onPointSelected, this,
          &MainWindow::pointSelected);
  connect(m_constraintsByAxis, SIGNAL(currentIndexChanged(int)), this,
          SLOT(constraintsIndexChanged(int)));

  connect(m_uDisplacmentTextEdit, &QLineEdit::textChanged, this,
          &MainWindow::uDisplacmentChanged);
  connect(m_vDisplacmentTextEdit, &QLineEdit::textChanged, this,
          &MainWindow::vDisplacmentChanged);

  connect(m_applySettings, &QPushButton::pressed, this,
          &MainWindow::applySettings);
  connect(m_computeButton, &QPushButton::pressed, this,
          &MainWindow::computeDisplacment);
}

void MainWindow::createMenuBar() {
  QMenuBar *menuBar = this->menuBar();

  QMenu *fileMenu = menuBar->addMenu("File");

  QAction *openAction = fileMenu->addAction("Open");

  QObject::connect(openAction, &QAction::triggered, this, &MainWindow::open);
}

void MainWindow::disableUI() {
  m_xCoordTextEdit->setEnabled(false);
  m_yCoordTextEdit->setEnabled(false);
  m_uDisplacmentTextEdit->setEnabled(false);
  m_vDisplacmentTextEdit->setEnabled(false);
  m_constraintsByAxis->setEnabled(false);
}

void MainWindow::enableUI() {
  m_xCoordTextEdit->setEnabled(true);
  m_yCoordTextEdit->setEnabled(true);
  m_uDisplacmentTextEdit->setEnabled(true);
  m_vDisplacmentTextEdit->setEnabled(true);
  m_constraintsByAxis->setEnabled(true);
}

void MainWindow::pointSelected(const int pointIndex) {
  qDebug() << pointIndex;

  if (pointIndex == -1) {
    disableUI();
    return;
  }

  enableUI();

  m_selectedPointIndex = pointIndex;

  const double x = m_points[pointIndex].x();
  const double y = m_points[pointIndex].y();

  m_xCoordTextEdit->setText(QString::number(x));
  m_yCoordTextEdit->setText(QString::number(y));

  bool isDisplacmentFound = false;
  bool isConstraintsFound = false;

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

      m_uDisplacmentTextEdit->setText(QString::number(u));
      m_vDisplacmentTextEdit->setText(QString::number(v));

      isDisplacmentFound = true;

      break;
    }
  }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex) {
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      StiffnessUtils::Constraints::Type constraintsType =
          m_constraints[constraintsIndex].type;
      m_constraintsByAxis->setCurrentIndex(static_cast<int>(constraintsType));

      m_uDisplacmentTextEdit->setEnabled(false);
      m_vDisplacmentTextEdit->setEnabled(false);

      isConstraintsFound = true;

      break;
    }
  }

  if (!isDisplacmentFound) {
    m_uDisplacmentTextEdit->setText(QString::number(0));
    m_vDisplacmentTextEdit->setText(QString::number(0));

    m_uDisplacmentTextEdit->setEnabled(true);
    m_vDisplacmentTextEdit->setEnabled(true);
  }

  if (!isConstraintsFound) {
    m_constraintsByAxis->setCurrentIndex(0);

    m_uDisplacmentTextEdit->setEnabled(true);
    m_vDisplacmentTextEdit->setEnabled(true);
  }
}

void MainWindow::constraintsIndexChanged(int index) {
  m_type = StiffnessUtils::Constraints::Type(index);

  if (m_type != StiffnessUtils::Constraints::Type::None) {
    m_uDisplacmentTextEdit->setEnabled(false);
    m_vDisplacmentTextEdit->setEnabled(false);
  } else {
    m_uDisplacmentTextEdit->setEnabled(true);
    m_vDisplacmentTextEdit->setEnabled(true);
  }
}

void MainWindow::uDisplacmentChanged(const QString &u) { m_u = u.toFloat(); }

void MainWindow::vDisplacmentChanged(const QString &v) { m_v = v.toFloat(); }

void MainWindow::applySettings() {
  if (m_selectedPointIndex == -1)
    return;

  for (int displacmentIndex = 0; displacmentIndex < m_displacments.size();
       ++displacmentIndex)
    if (m_displacments[displacmentIndex].pointIndex == m_selectedPointIndex) {
      m_displacments[displacmentIndex].u = m_u;
      m_displacments[displacmentIndex].v = m_v;
      break;
    }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex)
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      m_constraints[constraintsIndex].type = StiffnessUtils::Constraints::Type(
          m_constraintsByAxis->currentIndex());
      return;
    }

  StiffnessUtils::Displacment displacment;
  displacment.u = m_u;
  displacment.v = m_v;
  displacment.pointIndex = m_selectedPointIndex;
  m_displacments.push_back(displacment);

  if (m_constraintsByAxis->currentIndex() == 0)
    return;

  StiffnessUtils::Constraints constraints;
  constraints.type =
      StiffnessUtils::Constraints::Type(m_constraintsByAxis->currentIndex());
  constraints.pointIndex = m_selectedPointIndex;
  m_constraints.push_back(constraints);
}

void MainWindow::computeDisplacment() {
  float e = m_ETextEdit->text().toFloat();
  float v = m_vTextEdit->text().toFloat();

  StiffnessUtils::compute(e, v, m_points, m_indices, m_displacments,
                          m_constraints);

  m_viewport->setPoints(m_points);
  m_viewport->setTriangleIndices(m_indices);
  m_viewport->update();
}

void MainWindow::open() {
  if (m_loader.isRunning()) {
    QMessageBox::warning(this, "warning!",
                         "Unable to load new file, while loading: " +
                             m_loader.getPath());
    return;
  }

  const QString file = QFileDialog::getOpenFileName(this);

  if (file.isEmpty()) {
    return;
  }

  m_loader.setPath(file);
  m_loader.load();
}

void MainWindow::loaded() {
  m_points = m_loader.getVertices();

  std::pair<QVector<int>, QVector<QPointF>> indicesPoints =
      TriangulationUtils::triangulationPolygon(m_points, 2);
  m_indices = indicesPoints.first;
  m_points = indicesPoints.second;

  m_viewport->setPoints(m_points);
  m_viewport->setTriangleIndices(m_indices);
  m_viewport->update();
}

void MainWindow::failed() {
  QMessageBox::warning(this, "warning!",
                       "Unable to load file: " + m_loader.getPath());
}

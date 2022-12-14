#include "mainwindow.h"
#include "loader.h"
#include "viewport.h"

#include "cuthillmckeeutils.h"
#include "triangulationutils.h"

#include <iostream>
#include <vector>

using namespace std;

using namespace StiffnessUtils;
using namespace TriangulationUtils;
using namespace CuthillMckeeUtils;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
  setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
  setFixedSize(1000, 500);

  m_points << QPointF(0.0, 0.0) << QPointF(0.0, 1.0) << QPointF(1.0, 0.0)
           << QPointF(1.0, 1.0);
  m_indices << 0 << 1 << 2 << 1 << 2 << 3;

  m_loader.load(QString(PROJECT_SOURCE_DIR) + "/geometry.txt");
  m_points = m_loader.vertices();

  std::pair<QVector<int>, QVector<QPointF>> indicesPoints =
      triangulationPolygon(m_points, 2);
  m_indices = indicesPoints.first;
  m_points = indicesPoints.second;

  m_viewport = new Viewport();
  m_viewport->setPoints(m_points);
  m_viewport->setTriangleIndices(m_indices);
  m_viewport->update();

  //
  m_ELabel = new QLabel("E: ");
  m_ETextEdit = new QLineEdit;
  m_ETextEdit->setText("1");
  QHBoxLayout *materialELayout = new QHBoxLayout;
  materialELayout->addWidget(m_ELabel);
  materialELayout->addWidget(m_ETextEdit);

  m_vLabel = new QLabel("v: ");
  m_vTextEdit = new QLineEdit;
  m_vTextEdit->setText("0.3");
  QHBoxLayout *materialVLayout = new QHBoxLayout;
  materialVLayout->addWidget(m_vLabel);
  materialVLayout->addWidget(m_vTextEdit);

  m_xCoordLabel = new QLabel("x: ");
  m_xCoordTextEdit = new QLineEdit;
  QHBoxLayout *coordXLayout = new QHBoxLayout;
  coordXLayout->addWidget(m_xCoordLabel);
  coordXLayout->addWidget(m_xCoordTextEdit);

  m_yCoordLabel = new QLabel("y: ");
  m_yCoordTextEdit = new QLineEdit;
  QHBoxLayout *coordYLayout = new QHBoxLayout;
  coordYLayout->addWidget(m_yCoordLabel);
  coordYLayout->addWidget(m_yCoordTextEdit);

  m_uDisplacmentLabel = new QLabel("u: ");
  m_uDisplacmentTextEdit = new QLineEdit;
  m_uDisplacmentTextEdit->setText("0");
  QHBoxLayout *uDisplacmentLayout = new QHBoxLayout;
  uDisplacmentLayout->addWidget(m_uDisplacmentLabel);
  uDisplacmentLayout->addWidget(m_uDisplacmentTextEdit);

  m_vDisplacmentLabel = new QLabel("v: ");
  m_vDisplacmentTextEdit = new QLineEdit;
  m_vDisplacmentTextEdit->setText("0");
  QHBoxLayout *vDisplacmentLayout = new QHBoxLayout;
  vDisplacmentLayout->addWidget(m_vDisplacmentLabel);
  vDisplacmentLayout->addWidget(m_vDisplacmentTextEdit);

  m_applySettings = new QPushButton("Apply");
  m_computeButton = new QPushButton("Compute");

  m_constraintsByAxisLabel = new QLabel("Constraints: ");
  m_constraintsByAxis = new QComboBox;
  m_constraintsByAxis->addItems(QStringList() << "None"
                                              << "X"
                                              << "Y"
                                              << "XY");
  //

  QVBoxLayout *controlsLayout = new QVBoxLayout;
  controlsLayout->addLayout(materialELayout);
  controlsLayout->addLayout(materialVLayout);
  controlsLayout->addLayout(coordXLayout);
  controlsLayout->addLayout(coordYLayout);
  controlsLayout->addLayout(uDisplacmentLayout);
  controlsLayout->addLayout(vDisplacmentLayout);
  controlsLayout->addWidget(m_constraintsByAxisLabel);
  controlsLayout->addWidget(m_constraintsByAxis);
  controlsLayout->addWidget(m_applySettings);
  controlsLayout->addWidget(m_computeButton);

  QHBoxLayout *viewportControlLayout = new QHBoxLayout;
  viewportControlLayout->addWidget(m_viewport, 1);
  viewportControlLayout->addLayout(controlsLayout);

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

MainWindow::~MainWindow() {}

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

  const float x = m_points[pointIndex].x();
  const float y = m_points[pointIndex].y();

  m_xCoordTextEdit->setText(QString::number(x));
  m_yCoordTextEdit->setText(QString::number(y));

  bool isDisplacmentFound = false;
  bool isConstraintsFound = false;

  for (int displacmentIndex = 0; displacmentIndex < m_displacments.size();
       ++displacmentIndex)
    if (m_displacments[displacmentIndex].pointIndex == m_selectedPointIndex) {
      const float u = m_displacments[displacmentIndex].u;
      const float v = m_displacments[displacmentIndex].v;

      m_uDisplacmentTextEdit->setText(QString::number(u));
      m_vDisplacmentTextEdit->setText(QString::number(v));

      isDisplacmentFound = true;

      break;
    }

  for (int constraintsIndex = 0; constraintsIndex < m_constraints.size();
       ++constraintsIndex)
    if (m_constraints[constraintsIndex].pointIndex == m_selectedPointIndex) {
      StiffnessUtils::Constraints::Type constraintsType =
          m_constraints[constraintsIndex].type;
      m_constraintsByAxis->setCurrentIndex((int)constraintsType);

      m_uDisplacmentTextEdit->setEnabled(false);
      m_vDisplacmentTextEdit->setEnabled(false);

      isConstraintsFound = true;

      break;
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

  Displacment displacment;
  displacment.u = m_u;
  displacment.v = m_v;
  displacment.pointIndex = m_selectedPointIndex;
  m_displacments.push_back(displacment);

  if (m_constraintsByAxis->currentIndex() == 0)
    return;

  Constraints constraints;
  constraints.type =
      StiffnessUtils::Constraints::Type(m_constraintsByAxis->currentIndex());
  constraints.pointIndex = m_selectedPointIndex;
  m_constraints.push_back(constraints);
}

void MainWindow::computeDisplacment() {
  StiffnessUtils::compute(70000.0f, 0.3f, m_points, m_indices, m_displacments,
                          m_constraints);

  m_viewport->setPoints(m_points);
  m_viewport->setTriangleIndices(m_indices);
  m_viewport->update();
}

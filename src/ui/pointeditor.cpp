#include "pointeditor.h"

#include <QHBoxLayout>
#include <QLineEdit>

PointEditor::PointEditor(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f) {
  this->createLayout();
  this->disableUI();
}

void PointEditor::disableUI() {
  m_xCoordTextEdit->setEnabled(false);
  m_yCoordTextEdit->setEnabled(false);
  m_uDisplacmentTextEdit->setEnabled(false);
  m_vDisplacmentTextEdit->setEnabled(false);
  m_constraintsByAxis->setEnabled(false);
}

void PointEditor::enableUI() {
  m_xCoordTextEdit->setEnabled(true);
  m_yCoordTextEdit->setEnabled(true);
  m_uDisplacmentTextEdit->setEnabled(true);
  m_vDisplacmentTextEdit->setEnabled(true);
  m_constraintsByAxis->setEnabled(true);
}

void PointEditor::createLayout() {
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

  QHBoxLayout *viewportControlLayout = new QHBoxLayout;
  viewportControlLayout->addLayout(controlsLayout);

  this->setLayout(viewportControlLayout);
}

PointInfo PointEditor::getPointInfo() const { return mPointInfo; }

void PointEditor::setPointInfo(PointInfo PointInfo) { mPointInfo = PointInfo; }

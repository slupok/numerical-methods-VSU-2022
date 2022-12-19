#include "pointeditor.h"

#include <QGridLayout>
#include <QLineEdit>
#include <Qt>

#include "stiffnessutils.h"

PointEditor::PointEditor(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f) {
  this->createLayout();
  this->createConnections();
  this->disableUI();
}

void PointEditor::disableUI() {
  mXCoordinateSpinBox->setEnabled(false);
  mYCoordinateSpinBox->setEnabled(false);
  mUDisplacementSpinBox->setEnabled(false);
  mVDisplacementSpinBox->setEnabled(false);
  mConstraints->setEnabled(false);
}

void PointEditor::enableUI() {
  mXCoordinateSpinBox->setEnabled(true);
  mYCoordinateSpinBox->setEnabled(true);
  mUDisplacementSpinBox->setEnabled(true);
  mVDisplacementSpinBox->setEnabled(true);
  mConstraints->setEnabled(true);
}

void PointEditor::createLayout() {
  QGridLayout *mainGrid = new QGridLayout();
  mainGrid->setAlignment(Qt::AlignCenter);

  {
    QLabel *coordinateLabel = new QLabel("Coordinate");
    coordinateLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(coordinateLabel, 0, 0,
                        Qt::AlignVCenter | Qt::AlignRight);

    // x
    QLabel *xLabel = new QLabel("X");
    xLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(xLabel, 0, 1, Qt::AlignVCenter | Qt::AlignRight);

    mXCoordinateSpinBox = new QDoubleSpinBox;
    mainGrid->addWidget(mXCoordinateSpinBox, 0, 2, Qt::AlignCenter);

    // y
    QLabel *yLabel = new QLabel("Y");
    yLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(yLabel, 1, 1, Qt::AlignVCenter | Qt::AlignRight);

    mYCoordinateSpinBox = new QDoubleSpinBox;
    mainGrid->addWidget(mYCoordinateSpinBox, 1, 2, Qt::AlignCenter);
  }

  {
    QLabel *displacementLabel = new QLabel("Displacement");
    displacementLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(displacementLabel, 2, 0,
                        Qt::AlignVCenter | Qt::AlignRight);

    // u
    QLabel *uLabel = new QLabel("U");
    uLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(uLabel, 2, 1, Qt::AlignVCenter | Qt::AlignRight);

    mUDisplacementSpinBox = new QDoubleSpinBox;
    mainGrid->addWidget(mUDisplacementSpinBox, 2, 2, Qt::AlignCenter);

    // v
    QLabel *vLabel = new QLabel("V");
    vLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(vLabel, 3, 1, Qt::AlignVCenter | Qt::AlignRight);

    mVDisplacementSpinBox = new QDoubleSpinBox;
    mainGrid->addWidget(mVDisplacementSpinBox, 3, 2, Qt::AlignCenter);
  }

  {
    QLabel *constraintsLabel = new QLabel("Constraint");
    constraintsLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(constraintsLabel, 4, 0, 4, 1,
                        Qt::AlignVCenter | Qt::AlignRight);

    QStringList types;

    for (const auto &type : StiffnessUtils::Constraints::typesString) {
      types << type;
    }

    mConstraints = new QComboBox;
    mConstraints->addItems(types);
    mainGrid->addWidget(mConstraints, 4, 1, 4, 2, Qt::AlignCenter);
  }

  this->setLayout(mainGrid);
}

void PointEditor::createConnections() {
  // Coordinate
  QObject::connect(mXCoordinateSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mPointInfo.coordinate.setX(value);

                     emit pointChanged(mPointInfo);
                   });
  QObject::connect(mYCoordinateSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mPointInfo.coordinate.setY(value);

                     emit pointChanged(mPointInfo);
                   });

  // Displacement
  QObject::connect(mUDisplacementSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mPointInfo.displacement.setX(value);

                     emit pointChanged(mPointInfo);
                   });
  QObject::connect(mVDisplacementSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mPointInfo.displacement.setY(value);

                     emit pointChanged(mPointInfo);
                   });

  // Constraint
  QObject::connect(mConstraints,
                   qOverload<int>(&QComboBox::currentIndexChanged), this,
                   [this](const int &value) {
                     mPointInfo.constraintType =
                         static_cast<StiffnessUtils::Constraints::Type>(value);

                     emit pointChanged(mPointInfo);
                   });
}

PointInfo PointEditor::getPointInfo() const { return mPointInfo; }

void PointEditor::setPointInfo(PointInfo PointInfo) { mPointInfo = PointInfo; }

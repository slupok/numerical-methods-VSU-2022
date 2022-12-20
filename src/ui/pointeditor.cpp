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
  QGridLayout *mainGrid = new QGridLayout(this);
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
    mXCoordinateSpinBox->setSingleStep(0.1);
    mXCoordinateSpinBox->setMaximum(std::numeric_limits<double>::max());
    mXCoordinateSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    mainGrid->addWidget(mXCoordinateSpinBox, 0, 2, Qt::AlignCenter);

    // y
    QLabel *yLabel = new QLabel("Y");
    yLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(yLabel, 1, 1, Qt::AlignVCenter | Qt::AlignRight);

    mYCoordinateSpinBox = new QDoubleSpinBox;
    mYCoordinateSpinBox->setSingleStep(0.1);
    mYCoordinateSpinBox->setMaximum(std::numeric_limits<double>::max());
    mYCoordinateSpinBox->setMinimum(std::numeric_limits<double>::lowest());
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
    mUDisplacementSpinBox->setSingleStep(0.1);
    mUDisplacementSpinBox->setMaximum(std::numeric_limits<double>::max());
    mUDisplacementSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    mainGrid->addWidget(mUDisplacementSpinBox, 2, 2, Qt::AlignCenter);

    // v
    QLabel *vLabel = new QLabel("V");
    vLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(vLabel, 3, 1, Qt::AlignVCenter | Qt::AlignRight);

    mVDisplacementSpinBox = new QDoubleSpinBox;
    mVDisplacementSpinBox->setSingleStep(0.1);
    mVDisplacementSpinBox->setMaximum(std::numeric_limits<double>::max());
    mVDisplacementSpinBox->setMinimum(std::numeric_limits<double>::lowest());
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
}

void PointEditor::createConnections() {
  // Coordinate
  QObject::connect(mXCoordinateSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mInfo.coordinate.setX(value);

                     emit pointInfoChanged(mInfo);
                   });
  QObject::connect(mYCoordinateSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mInfo.coordinate.setY(value);

                     emit pointInfoChanged(mInfo);
                   });

  // Displacement
  QObject::connect(mUDisplacementSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mInfo.displacement.setX(value);

                     emit pointInfoChanged(mInfo);
                   });
  QObject::connect(mVDisplacementSpinBox,
                   qOverload<double>(&QDoubleSpinBox::valueChanged), this,
                   [this](const double &value) {
                     mInfo.displacement.setY(value);

                     emit pointInfoChanged(mInfo);
                   });

  // Constraint
  QObject::connect(mConstraints,
                   qOverload<int>(&QComboBox::currentIndexChanged), this,
                   [this](const int &value) {
                     mInfo.constraintType =
                         static_cast<StiffnessUtils::Constraints::Type>(value);

                     emit pointInfoChanged(mInfo);
                   });
}

PointInfo PointEditor::getPointInfo() const { return mInfo; }

void PointEditor::setPointInfo(PointInfo PointInfo) {
  mInfo = PointInfo;

  mXCoordinateSpinBox->setValue(mInfo.coordinate.x());
  mYCoordinateSpinBox->setValue(mInfo.coordinate.y());
  mUDisplacementSpinBox->setValue(mInfo.displacement.x());
  mVDisplacementSpinBox->setValue(mInfo.displacement.y());
  mConstraints->setCurrentIndex(static_cast<int>(mInfo.constraintType));
}

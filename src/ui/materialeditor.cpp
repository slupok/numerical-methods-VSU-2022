#include "materialeditor.h"

#include <QHBoxLayout>
#include <QLabel>

MaterialEditor::MaterialEditor(QWidget *parent, Qt::WindowFlags f)
    : QWidget(parent, f) {
  this->createLayout();
  this->createConnections();
}

void MaterialEditor::createLayout() {
  QGridLayout *mainGrid = new QGridLayout(this);
  mainGrid->setAlignment(Qt::AlignCenter);

  {
    // E
    QLabel *eLabel = new QLabel("E");
    eLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(eLabel, 0, 1, Qt::AlignVCenter | Qt::AlignRight);

    mESpinBox = new QDoubleSpinBox;
    mESpinBox->setSingleStep(0.1);
    mESpinBox->setMaximum(std::numeric_limits<double>::max());
    mESpinBox->setMinimum(std::numeric_limits<double>::lowest());
    mainGrid->addWidget(mESpinBox, 0, 2, Qt::AlignCenter);
  }

  {
    // v
    QLabel *vLabel = new QLabel("v");
    vLabel->setAlignment(Qt::AlignRight);
    mainGrid->addWidget(vLabel, 1, 1, Qt::AlignVCenter | Qt::AlignRight);

    mVSpinBox = new QDoubleSpinBox;
    mVSpinBox->setSingleStep(0.1);
    mVSpinBox->setMaximum(std::numeric_limits<double>::max());
    mVSpinBox->setMinimum(std::numeric_limits<double>::lowest());
    mainGrid->addWidget(mVSpinBox, 1, 2, Qt::AlignCenter);
  }
}

void MaterialEditor::createConnections() {
  QObject::connect(mESpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
                   this, [this](const double &value) {
                     mMaterial.e = static_cast<float>(value);

                     emit materialChanged(mMaterial);
                   });
  QObject::connect(mVSpinBox, qOverload<double>(&QDoubleSpinBox::valueChanged),
                   this, [this](const double &value) {
                     mMaterial.v = static_cast<float>(value);

                     emit materialChanged(mMaterial);
                   });
}

const Material &MaterialEditor::getMaterialInfo() const { return mMaterial; }

void MaterialEditor::setMaterialInfo(const Material &Info) {
  mMaterial = Info;

  mESpinBox->setValue(mMaterial.e);
  mVSpinBox->setValue(mMaterial.v);
}

void MaterialEditor::disableUI() {
  mESpinBox->setEnabled(false);
  mVSpinBox->setEnabled(false);
}

void MaterialEditor::enableUI() {
  mESpinBox->setEnabled(true);
  mVSpinBox->setEnabled(true);
}

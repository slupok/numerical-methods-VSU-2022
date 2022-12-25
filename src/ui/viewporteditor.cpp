#include "viewporteditor.h"

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

ViewportEditor::ViewportEditor(const bool showFigure,
                               const bool showSolvedFigure, QWidget *parent,
                               Qt::WindowFlags f)
    : QWidget(parent, f) {
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->setAlignment(Qt::AlignCenter);

  {
    QHBoxLayout *showFigureLayout = new QHBoxLayout;

    QLabel *showFigureLabel = new QLabel("Show figure");
    showFigureLabel->setAlignment(Qt::AlignRight);
    showFigureLayout->addWidget(showFigureLabel,
                                Qt::AlignVCenter | Qt::AlignRight);

    QCheckBox *showFigureCheckBox = new QCheckBox;
    showFigureCheckBox->setCheckState(showFigure ? Qt::CheckState::Checked
                                                 : Qt::CheckState::Unchecked);
    QObject::connect(showFigureCheckBox, &QCheckBox::stateChanged, this,
                     [this](int state) {
                       emit showFigureChanged(
                           state == Qt::CheckState::Checked ? true : false);
                     });
    showFigureLayout->addWidget(showFigureCheckBox, Qt::AlignCenter);

    mainLayout->addLayout(showFigureLayout, Qt::AlignCenter);
  }

  {
    QHBoxLayout *showSolvedFigureLayout = new QHBoxLayout;

    QLabel *showSolvedFigureLabel = new QLabel("Show solved figure");
    showSolvedFigureLabel->setAlignment(Qt::AlignRight);
    showSolvedFigureLayout->addWidget(showSolvedFigureLabel,
                                      Qt::AlignVCenter | Qt::AlignRight);

    QCheckBox *showSolvedFigureCheckBox = new QCheckBox;
    showSolvedFigureCheckBox->setCheckState(
        showSolvedFigure ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    QObject::connect(showSolvedFigureCheckBox, &QCheckBox::stateChanged, this,
                     [this](int state) {
                       emit showSolvedFigureChange(
                           state == Qt::CheckState::Checked ? true : false);
                     });
    showSolvedFigureLayout->addWidget(showSolvedFigureCheckBox,
                                      Qt::AlignCenter);

    mainLayout->addLayout(showSolvedFigureLayout, Qt::AlignCenter);
  }
}

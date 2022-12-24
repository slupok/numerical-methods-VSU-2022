#ifndef MATERIALEDITOR_H
#define MATERIALEDITOR_H

#include <QDoubleSpinBox>
#include <QWidget>

#include "common/material.h"

class MaterialEditor : public QWidget {
  Q_OBJECT
public:
  explicit MaterialEditor(QWidget *parent = nullptr,
                          Qt::WindowFlags f = Qt::WindowFlags());

  const Material &getMaterialInfo() const;
  void setMaterialInfo(const Material &Info);

signals:
  void materialChanged(const Material &info);

public slots:
  void disableUI();
  void enableUI();

private:
  void createLayout();
  void createConnections();

  // Material settings
  QDoubleSpinBox *mESpinBox = nullptr;
  QDoubleSpinBox *mVSpinBox = nullptr;

  Material mMaterial;
};

#endif // MATERIALEDITOR_H

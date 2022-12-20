#ifndef MATERIALEDITOR_H
#define MATERIALEDITOR_H

#include <QDoubleSpinBox>
#include <QWidget>

#include "common/materialinfo.h"

class MaterialEditor : public QWidget {
  Q_OBJECT
public:
  explicit MaterialEditor(QWidget *parent = nullptr,
                          Qt::WindowFlags f = Qt::WindowFlags());

  const MaterialInfo &getMaterialInfo() const;
  void setMaterialInfo(const MaterialInfo &Info);

signals:
  void materialInfoChanged(const MaterialInfo &info);

public slots:
  void disableUI();
  void enableUI();

private:
  void createLayout();
  void createConnections();

  // Material settings
  QDoubleSpinBox *mESpinBox = nullptr;
  QDoubleSpinBox *mVSpinBox = nullptr;

  MaterialInfo mInfo;
};

#endif // MATERIALEDITOR_H

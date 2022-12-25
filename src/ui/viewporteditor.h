#ifndef VIEWPORTEDITOR_H
#define VIEWPORTEDITOR_H

#include <QCheckBox>
#include <QWidget>

class ViewportEditor : public QWidget {
  Q_OBJECT
public:
  ViewportEditor(const bool showFigure, const bool showSolvedFigure,
                 QWidget *parent = nullptr,
                 Qt::WindowFlags f = Qt::WindowFlags());

signals:
  void showFigureChanged(const bool showFigure);
  void showSolvedFigureChange(const bool showSolvedFigure);

private:
};

#endif // VIEWPORTEDITOR_H

#ifndef LOADER_H
#define LOADER_H

#include <QString>
#include <QVector2D>
#include <QVector>

class Loader {
public:
  Loader() = default;
  Loader(const QString &path);

  void load(const QString &path);

  QVector<QPointF> vertices() const;

private:
  void parseVertices(const QString &vertex);

  QVector<QPointF> m_vertices;
};

#endif // LOADER_H

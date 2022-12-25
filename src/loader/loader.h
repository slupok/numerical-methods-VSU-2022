#ifndef LOADER_H
#define LOADER_H

#include <QString>
#include <QVector2D>
#include <QVector>

class Loader {
public:
  Loader() = default;
  Loader(const QString &path);

  bool load();

  const QVector<QPointF> &getVertices() const;

  const QString &getPath() const;
  void setPath(const QString &path);

private:
  bool parseVertices(const QString &vertex);

  QVector<QPointF> mVertices;

  QString mPath;
};

#endif // LOADER_H

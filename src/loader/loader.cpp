#include "loader.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

Loader::Loader(const QString &path) : mPath(path) {}

bool Loader::load() {
  if (mPath.isEmpty()) {
    return false;
  }

  QFile file(mPath);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  mVertices.clear();

  QTextStream in(&file);

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (line[0] != 'v' || !parseVertices(line)) {
      mVertices.clear();
      return false;
    }
  }

  return true;
}

const QVector<QPointF> &Loader::getVertices() const { return mVertices; }

bool Loader::parseVertices(const QString &vertex) {
  QStringList splittedVertex = vertex.split(" ");

  // v x y
  if (splittedVertex.size() != 3) {
    return false;
  }

  bool xOk;
  bool yOk;

  float x = splittedVertex[1].toFloat(&xOk);
  float y = splittedVertex[2].toFloat(&yOk);

  if (!xOk || !yOk) {
    return false;
  }

  mVertices.push_back(QPointF(x, y));

  return true;
}

const QString &Loader::getPath() const { return mPath; }

void Loader::setPath(const QString &path) { mPath = path; }

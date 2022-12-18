#include "loader.h"

#include <QFile>
#include <QStringList>
#include <QTextStream>

Loader::Loader(const QString &path) : m_path(path) {}

bool Loader::load() {
  if (m_path.isEmpty()) {
    return false;
  }

  QFile file(m_path);

  if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
    return false;
  }

  QTextStream in(&file);

  while (!in.atEnd()) {
    QString line = in.readLine();

    if (line[0] != 'v' || !parseVertices(line)) {
      m_vertices.clear();
      return false;
    }
  }

  return true;
}

const QVector<QPointF> &Loader::getVertices() const { return m_vertices; }

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

  m_vertices.push_back(QPointF(x, y));

  return true;
}

const QString &Loader::getPath() const { return m_path; }

void Loader::setPath(const QString &path) { m_path = path; }

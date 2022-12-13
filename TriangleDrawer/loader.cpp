#include "loader.h"

#include <QStringList>
#include <QFile>
#include <QTextStream>

Loader::Loader(const QString &path)
{
    load(path);
}

void Loader::load(const QString &path)
{
   QFile file(path);
   if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
       return;

   QTextStream in(&file);
   while (!in.atEnd()) {
       QString line = in.readLine();
       if (line[0] == 'v')
           parseVertices(line);
   }
}

QVector<QPointF> Loader::vertices() const
{
    return m_vertices;
}

void Loader::parseVertices(const QString &vertex)
{
    QStringList splittedVertex = vertex.split(" ");

    float x = splittedVertex[1].toFloat();
    float y = splittedVertex[2].toFloat();

    m_vertices.push_back(QPointF(x, y));
}

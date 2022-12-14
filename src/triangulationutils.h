#ifndef TRIANGULATIONUTILS_H
#define TRIANGULATIONUTILS_H

#include <QPointF>
#include <QVector>
#include <utility>
#include <vector>

namespace TriangulationUtils {

#define M_PI 3.14159265358979323846

struct PointWithLinks {
  float x = 0.0f;
  float y = 0.0f;
  int currentNumber;

  std::vector<int> links;

  PointWithLinks(const PointWithLinks &point) {
    x = point.x;
    y = point.y;
    for (const auto &link : point.links) {
      links.push_back(link);
    }

    currentNumber = point.currentNumber;
  }

  PointWithLinks(float _x, float _y) {
    x = _x;
    y = _y;
  }

  PointWithLinks(float _x, float _y, int _number, int _max) {
    x = _x;
    y = _y;
    currentNumber = _number;

    if (_number == 0) {
      links.push_back(_max - 1);
      links.push_back(_number + 1);
    } else {
      if (_number == _max - 1) {
        links.push_back(_number - 1);
        links.push_back(0);
      } else {
        links.push_back(_number - 1);
        links.push_back(_number + 1);
      }
    }
  }

  PointWithLinks() {}
};

struct Triangle {
  int firstVertex;
  int secondVertex;
  int thirdVertex;

  Triangle(int _first, int _second, int _third) {
    firstVertex = _first;
    secondVertex = _second;
    thirdVertex = _third;
  }
};

struct Vector2 {
  float x = 0.0f;
  float y = 0.0f;

  Vector2() {}

  Vector2(float _x, float _y) {
    x = _x;
    y = _y;
  }
};

float AngleBetween(PointWithLinks vector1, PointWithLinks vector2);
PointWithLinks medianVector(PointWithLinks pointA, PointWithLinks pointB,
                            float k);
std::pair<int, float>
findMinAngle(const std::vector<PointWithLinks> &localPoints,
             const std::vector<PointWithLinks> &points);
std::vector<Triangle> Triangulate(std::vector<PointWithLinks> &points, float k,
                                  float mainAngle, float rightAngle,
                                  float auxiliaryAngle);
void Removal(std::vector<PointWithLinks> &points,
             std::vector<PointWithLinks> &localPoints,
             std::vector<Triangle> &triangles, int index);
void Alignment(std::vector<PointWithLinks> &points,
               std::vector<PointWithLinks> &localPoints,
               std::vector<Triangle> &triangles, int index, float k);
int FindByCurrentNumber(std::vector<PointWithLinks> &localPoints, int index);
int FindLastByCurrentNumber(std::vector<PointWithLinks> &localPoints,
                            int index);
int FindLastByCurrentNumberSkippingOne(std::vector<PointWithLinks> &localPoints,
                                       int index);

void VisitEdge(int startVerticesCount, std::vector<Vector2> &oldVertices,
               std::vector<Vector2> &newVertices,
               std::vector<int> &newVerticesNeighbors, int point1, int point2);
void Relax(int startVerticesCount, std::vector<Vector2> &vertices,
           std::vector<int> &triangles);

std::pair<QVector<int>, QVector<QPointF>>
triangulationPolygon(const QVector<QPointF> &points, const int relaxationStep);
std::vector<PointWithLinks> pointsWithLinks(const QVector<QPointF> &points);

} // namespace TriangulationUtils

#endif // TRIANGULATIONUTILS_H

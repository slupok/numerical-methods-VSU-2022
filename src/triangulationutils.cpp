#include "triangulationutils.h"

#include <cmath>

using namespace std;
using namespace TriangulationUtils;

float TriangulationUtils::AngleBetween(PointWithLinks vector1,
                                       PointWithLinks vector2) {

  float cos = (vector1.x * vector2.x + vector1.y * vector2.y) / sqrt((vector1.x * vector1.x + vector1.y * vector1.y) * (vector2.x * vector2.x + vector2.y * vector2.y));
    if(cos < -1)
        cos = -1;
    if(cos > 1)
        cos = 1;
    auto result = acos(cos) * (180 / M_PI);

  if (vector1.y * vector2.x - vector1.x * vector2.y < 0)
    result = 360 - result;

  return (float)result;
}

std::pair<int, float>
TriangulationUtils::findMinAngle(const std::vector<PointWithLinks> &localPoints,
                                 const std::vector<PointWithLinks> &points) {
  float minAngle = std::numeric_limits<float>::max();
  int index = -1;

  for (int i = 0; i < localPoints.size(); i++) {
    auto currentPoint = localPoints[i];
    auto indexOfPrev = currentPoint.links[0];
    auto indexOfNext = currentPoint.links[1];

    PointWithLinks prevPoint =
        PointWithLinks(points[indexOfPrev].x - currentPoint.x,
                       points[indexOfPrev].y - currentPoint.y);
    PointWithLinks nextPoint =
        PointWithLinks(points[indexOfNext].x - currentPoint.x,
                       points[indexOfNext].y - currentPoint.y);

    auto angle = AngleBetween(prevPoint, nextPoint);

    if (angle < minAngle) {
      minAngle = angle;
      index = i;
    }
  }

  return pair<int, float>(index, minAngle);
}

std::vector<Triangle>
TriangulationUtils::Triangulate(std::vector<PointWithLinks> &points, float k,
                                float mainAngle, float rightAngle,
                                float auxiliaryAngle) {
  vector<PointWithLinks> localPoints;
  for (const auto &point : points)
    localPoints.push_back(PointWithLinks(point));

  int i = 0;
  vector<Triangle> result;

  while (localPoints.size() > 3) {
    int index;
    float a1, a2, a3;
     pair<int, float> minAngle = findMinAngle(localPoints, points);

    index = minAngle.first;
    a1 = minAngle.second;

    if (a1 <= mainAngle) {
      Removal(points, localPoints, result, index);
    } else {
      auto currentPoint = localPoints[index];
      auto prevCurrentNumber = points[currentPoint.links[0]].currentNumber;
      auto nextCurrentNumber = points[currentPoint.links[1]].currentNumber;
      auto indexOfPrev = FindByCurrentNumber(localPoints, prevCurrentNumber);
      auto indexOfNext = FindByCurrentNumber(localPoints, nextCurrentNumber);

      PointWithLinks prevPoint = PointWithLinks(
          localPoints[indexOfPrev].x - localPoints[indexOfNext].x,
          localPoints[indexOfPrev].y - localPoints[indexOfNext].y);
      PointWithLinks nextPoint =
          PointWithLinks(localPoints[index].x - localPoints[indexOfNext].x,
                         localPoints[index].y - localPoints[indexOfNext].y);

      a2 = AngleBetween(prevPoint, nextPoint);
      a3 = 180 - a2 - a1;

      if ((a1 > mainAngle && a1 <= rightAngle) && a2 >= auxiliaryAngle &&
          a3 >= auxiliaryAngle) {
        Removal(points, localPoints, result, index);
      }
      if ((a1 > rightAngle) || a2 < auxiliaryAngle || a3 < auxiliaryAngle) {
        Alignment(points, localPoints, result, index, k);
      }
    }
    i++;
  }

  int firstPoint =
      FindLastByCurrentNumber(points, localPoints[0].currentNumber);
  int secondPoint =
      FindLastByCurrentNumber(points, localPoints[1].currentNumber);
  int thirdPoint =
      FindLastByCurrentNumber(points, localPoints[2].currentNumber);

  result.push_back(Triangle(firstPoint, secondPoint, thirdPoint));

  return result;
}

void TriangulationUtils::Removal(std::vector<PointWithLinks> &points,
                                 std::vector<PointWithLinks> &localPoints,
                                 std::vector<Triangle> &triangles, int index) {
  PointWithLinks pointToRemove = PointWithLinks(localPoints[index]);

  int prevCurrentNumber = points[pointToRemove.links[0]].currentNumber;
  int nextCurrentNumber = points[pointToRemove.links[1]].currentNumber;

  int indexOfPrev = FindByCurrentNumber(localPoints, prevCurrentNumber);
  int indexOfNext = FindByCurrentNumber(localPoints, nextCurrentNumber);

  int prevLastInPoints = FindLastByCurrentNumber(points, prevCurrentNumber);
  int nextLastInPoints = FindLastByCurrentNumber(points, nextCurrentNumber);
  int lastIndex = FindLastByCurrentNumber(points, pointToRemove.currentNumber);

  localPoints[indexOfPrev].links.erase(localPoints[indexOfPrev].links.begin() +
                                       1);
  localPoints[indexOfPrev].links.insert(
      localPoints[indexOfPrev].links.begin() + 1, nextLastInPoints);

  localPoints[indexOfNext].links.erase(localPoints[indexOfNext].links.begin());
  localPoints[indexOfNext].links.insert(localPoints[indexOfNext].links.begin(),
                                        prevLastInPoints);

  points[nextLastInPoints].links.push_back(prevLastInPoints);
  points[prevLastInPoints].links.push_back(nextLastInPoints);

  localPoints.erase(localPoints.begin() + index);

  triangles.push_back(Triangle(lastIndex, nextLastInPoints, prevLastInPoints));
}

void TriangulationUtils::Alignment(std::vector<PointWithLinks> &points,
                                   std::vector<PointWithLinks> &localPoints,
                                   std::vector<Triangle> &triangles, int index,
                                   float k) {
  PointWithLinks pointToRemove = PointWithLinks(localPoints[index]);
  int prevCurrentNumber = points[pointToRemove.links[0]].currentNumber;
  int nextCurrentNumber = points[pointToRemove.links[1]].currentNumber;

  int indexOfPrev = FindByCurrentNumber(localPoints, prevCurrentNumber);
  int indexOfNext = FindByCurrentNumber(localPoints, nextCurrentNumber);

  PointWithLinks prevPoint =
      PointWithLinks(localPoints[indexOfPrev].x - localPoints[index].x,
                     localPoints[indexOfPrev].y - localPoints[index].y);
  PointWithLinks nextPoint =
      PointWithLinks(localPoints[indexOfNext].x - localPoints[index].x,
                     localPoints[indexOfNext].y - localPoints[index].y);
  PointWithLinks newPoint = medianVector(prevPoint, nextPoint, k);

  PointWithLinks lastPoint = PointWithLinks(pointToRemove);
  pointToRemove.x += newPoint.x;
  pointToRemove.y += newPoint.y;
  
  for (int i = 0; i < localPoints.size(); i++) {

      int indexOfFirst = localPoints[i].currentNumber;
      int indexOfSecond = localPoints[i].links[1];

	if(indexOfFirst != index && indexOfSecond != index){
		float delta = (points[indexOfSecond].x - points[indexOfFirst].x)*(lastPoint.y - pointToRemove.y) - (points[indexOfSecond].y - points[indexOfFirst].y)*(lastPoint.x - pointToRemove.x);
		if(delta != 0){
			float delta1 = (lastPoint.x - points[indexOfFirst].x)*(lastPoint.y - pointToRemove.y) - (lastPoint.y - points[indexOfFirst].y)*(lastPoint.x - pointToRemove.x);
			float delta2 = (points[indexOfSecond].x - points[indexOfFirst].x)*(lastPoint.y - points[indexOfFirst].y) - (points[indexOfSecond].y - points[indexOfFirst].y)*(lastPoint.x - points[indexOfFirst].x);
			float t1 = delta1/delta;
			float t2 = delta2/delta;
			if(t1 > -0.1 && t1 < 1.1 && t2 > 0 && t2 < 2){
				pointToRemove.x = lastPoint.x * (1 - t2 * 0.5) + pointToRemove.x * t2 * 0.5;
				pointToRemove.y = lastPoint.y * (1 - t2 * 0.5) + pointToRemove.y * t2 * 0.5;
			}
		}
	}
}

  localPoints[index] = pointToRemove;
  points[pointToRemove.links[0]].links.push_back(points.size());
  points[pointToRemove.links[1]].links.push_back(points.size());
  points.push_back(PointWithLinks(pointToRemove));

  int prevLastInPoints = FindLastByCurrentNumber(points, prevCurrentNumber);
  int nextLastInPoints = FindLastByCurrentNumber(points, nextCurrentNumber);
  int lastIndex =
      FindLastByCurrentNumberSkippingOne(points, pointToRemove.currentNumber);
  points[points.size() - 1].links.push_back(lastIndex);

  localPoints[indexOfPrev].links.erase(localPoints[indexOfPrev].links.begin() +
                                       1);
  localPoints[indexOfPrev].links.insert(
      localPoints[indexOfPrev].links.begin() + 1, points.size() - 1);

  localPoints[indexOfNext].links.erase(localPoints[indexOfNext].links.begin());
  localPoints[indexOfNext].links.insert(localPoints[indexOfNext].links.begin(),
                                        points.size() - 1);

  triangles.push_back(Triangle(lastIndex, nextLastInPoints, points.size() - 1));
  triangles.push_back(Triangle(lastIndex, points.size() - 1, prevLastInPoints));
}

int TriangulationUtils::FindByCurrentNumber(
    std::vector<PointWithLinks> &localPoints, int index) {
  int pos = -1;

  for (int i = 0; i < localPoints.size(); i++) {
    if (localPoints[i].currentNumber == index) {
      return i;
    }
  }

  return pos;
}

int TriangulationUtils::FindLastByCurrentNumber(
    std::vector<PointWithLinks> &localPoints, int index) {
  int pos = -1;

  for (int i = 0; i < localPoints.size(); i++) {
    if (localPoints[i].currentNumber == index) {
      pos = i;
    }
  }

  return pos;
}

int TriangulationUtils::FindLastByCurrentNumberSkippingOne(
    std::vector<PointWithLinks> &localPoints, int index) {
  int pos = index;
  bool oneSkipped = false;

  for (int i = localPoints.size() - 1; i >= 0; i--) {
    if (localPoints[i].currentNumber == index) {
      if (oneSkipped == true) {
        pos = i;
        return pos;
      } else {
        oneSkipped = true;
      }
    }
  }

  return pos;
}

void TriangulationUtils::VisitEdge(int startVerticesCount,
                                   std::vector<Vector2> &oldVertices,
                                   std::vector<Vector2> &newVertices,
                                   std::vector<int> &newVerticesNeighbors,
                                   int point1, int point2) {
  if (point1 >= startVerticesCount) {
    if (point2 >= startVerticesCount) {
      newVertices[point2 - startVerticesCount].x += oldVertices[point1].x;
      newVertices[point2 - startVerticesCount].y += oldVertices[point1].y;

      newVerticesNeighbors[point2 - startVerticesCount]++;
    }
    newVertices[point1 - startVerticesCount].x += oldVertices[point2].x;
    newVertices[point1 - startVerticesCount].y += oldVertices[point2].y;

    newVerticesNeighbors[point1 - startVerticesCount]++;
  } else {
    if (point2 >= startVerticesCount) {
      newVertices[point2 - startVerticesCount].x += oldVertices[point1].x;
      newVertices[point2 - startVerticesCount].y += oldVertices[point1].y;

      newVerticesNeighbors[point2 - startVerticesCount]++;
    }
  }
}

void TriangulationUtils::Relax(int startVerticesCount,
                               std::vector<Vector2> &vertices,
                               std::vector<int> &triangles) {
  vector<Vector2> newVertices(vertices.size() - startVerticesCount);
  vector<int> newVerticesNeighbors(vertices.size() - startVerticesCount);
  for (int i = 0; i < triangles.size(); i += 3) {
    VisitEdge(startVerticesCount, vertices, newVertices, newVerticesNeighbors,
              triangles[i], triangles[i + 1]);
    VisitEdge(startVerticesCount, vertices, newVertices, newVerticesNeighbors,
              triangles[i + 1], triangles[i + 2]);
    VisitEdge(startVerticesCount, vertices, newVertices, newVerticesNeighbors,
              triangles[i + 2], triangles[i]);
  }
  for (int i = 0; i < newVertices.size(); ++i) {
    vertices[i + startVerticesCount].x =
        newVertices[i].x / newVerticesNeighbors[i];
    vertices[i + startVerticesCount].y =
        newVertices[i].y / newVerticesNeighbors[i];
  }
}

PointWithLinks TriangulationUtils::medianVector(PointWithLinks pointA,
                                                PointWithLinks pointB,
                                                float k) {
  PointWithLinks result;

    float ANorm = sqrt(pointA.x * pointA.x + pointA.y * pointA.y);
    float BNorm = sqrt(pointB.x * pointB.x + pointB.y * pointB.y);


  result.x = (pointA.x / ANorm + pointB.x / BNorm) * k * (ANorm + BNorm) / (1 + (pointA.x * pointB.x + pointA.y * pointB.y) / (ANorm * BNorm));
  result.y = (pointA.y / ANorm + pointB.y / BNorm) * k * (ANorm + BNorm) / (1 + (pointA.x * pointB.x + pointA.y * pointB.y) / (ANorm * BNorm));

  return result;
}

std::pair<QVector<int>, QVector<QPointF>>
TriangulationUtils::triangulationPolygon(const QVector<QPointF> &points,
                                         const int relaxationStep) {
  vector<PointWithLinks> pointsWidthLinks;

  for (int pointIndex = 0; pointIndex < points.size(); ++pointIndex)
    pointsWidthLinks.push_back(PointWithLinks(points[pointIndex].x(),
                                              points[pointIndex].y(),
                                              pointIndex, points.size()));

  vector<Triangle> triangles = TriangulationUtils::Triangulate(
      pointsWidthLinks, 16.0f, 75.0f, 90.0f, 30.0f);

  vector<Vector2> vectors(pointsWidthLinks.size());
  for (int i = 0; i < pointsWidthLinks.size(); i++)
    vectors[i] = Vector2(pointsWidthLinks[i].x, pointsWidthLinks[i].y);

  vector<int> newTriangles(triangles.size() * 3);
  int k = 0;
  for (int i = 0; i < newTriangles.size(); i += 3) {
    newTriangles[i] = triangles[k].firstVertex;
    newTriangles[i + 1] = triangles[k].secondVertex;
    newTriangles[i + 2] = triangles[k].thirdVertex;
    k++;
  }

  for (int relaxationIndex = 0; relaxationIndex < relaxationStep;
       ++relaxationIndex)
    Relax(pointsWidthLinks.size(), vectors, newTriangles);

  for (int i = points.size(); i < pointsWidthLinks.size(); i++) {
    pointsWidthLinks[i].x = vectors[i].x;
    pointsWidthLinks[i].y = vectors[i].y;
  }

  QVector<int> indices;
  for (int triangleIndex = 0; triangleIndex < triangles.size();
       ++triangleIndex) {
    indices.push_back(triangles[triangleIndex].firstVertex);
    indices.push_back(triangles[triangleIndex].secondVertex);
    indices.push_back(triangles[triangleIndex].thirdVertex);
  }

  QVector<QPointF> triangulationPoint;
  for (int pointIndex = 0; pointIndex < pointsWidthLinks.size(); ++pointIndex)
    triangulationPoint.push_back(QPointF(pointsWidthLinks[pointIndex].x,
                                         pointsWidthLinks[pointIndex].y));

  return std::pair<QVector<int>, QVector<QPointF>>(indices, triangulationPoint);
}

std::vector<PointWithLinks>
TriangulationUtils::pointsWithLinks(const QVector<QPointF> &points) {
  vector<PointWithLinks> pointsWidthLinks;

  for (int pointIndex = 0; pointIndex < points.size(); ++pointIndex)
    pointsWidthLinks.push_back(PointWithLinks(points[pointIndex].x(),
                                              points[pointIndex].y(),
                                              pointIndex, points.size()));

  vector<Triangle> triangles = TriangulationUtils::Triangulate(
      pointsWidthLinks, 16.0f, 75.0f, 90.0f, 30.0f);

  return pointsWidthLinks;
}

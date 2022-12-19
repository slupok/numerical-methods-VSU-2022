#ifndef STIFFNESSUTILS_H
#define STIFFNESSUTILS_H

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <QPointF>
#include <QVector>

namespace StiffnessUtils {

struct PointF {
  PointF() {}

  PointF(const float &_x, const float &_y) : x(_x), y(_y) {}

  float x = 0.0f;
  float y = 0.0f;
};

struct Displacment {
  float u = 0.0f;
  float v = 0.0f;

  int pointIndex = -1;
};

struct Constraints {
  enum class Type {
    None = 0,
    X = 1,
    Y = 2,
    XY = 3,
  };

  static const constexpr std::initializer_list<const char *const> typesString =
      {"None", "X", "Y", "XY"};

  Type type = Type::None;
  int pointIndex = -1;
};

void compute(const float &E, const float &v, QVector<QPointF> &points,
             const QVector<int> &indices,
             const QVector<Displacment> &displacments,
             const QVector<Constraints> &constraints);

Eigen::Matrix3f calculateMaterialMatrix(const float &E, const float &v);
void calculateLocalStiffnesMatrix(
    const Eigen::Matrix3f &D, const std::vector<PointF> &pointsOfLocalTriangle,
    const std::vector<int> &indicesOfLocalTriangle,
    std::vector<Eigen::Triplet<float>> &triplets);
Eigen::SparseMatrix<float> calculateGlobalStiffnesMatrix(
    const int pointsCount, const std::vector<Eigen::Triplet<float>> &triplets);

Eigen::VectorXf applyDisplacments(Eigen::SparseMatrix<float> &stiffnessMatrix,
                                  std::vector<bool> &maskedVertices,
                                  const std::vector<Displacment> &displasments);
void applyConstraints(Eigen::SparseMatrix<float> &stiffnessMatrix,
                      Eigen::VectorXf &displacments,
                      const std::vector<Constraints> &constraints);

Eigen::VectorXf CholetskySolver(const Eigen::SparseMatrix<float> &A,
                                const Eigen::VectorXf &b);

} // namespace StiffnessUtils

#endif // STIFFNESSUTILS_H

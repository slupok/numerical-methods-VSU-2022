#include "stiffnessutils.h"
#include <vector>

#include <iostream>

using namespace std;
using namespace Eigen;
using namespace StiffnessUtils;

class Matrx3f{
public:
    double **A;
    int N =100;

void inverse()
{
    double temp;
    double **E = new double *[N];

    for (int i = 0; i < N; i++)
        E[i] = new double [N];

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
        {
            E[i][j] = 0.0;

            if (i == j)
                E[i][j] = 1.0;
        }

    for (int k = 0; k < N; k++)
    {
        temp = A[k][k];

        for (int j = 0; j < N; j++)
        {
            A[k][j] /= temp;
            E[k][j] /= temp;
        }

        for (int i = k + 1; i < N; i++)
        {
            temp = A[i][k];

            for (int j = 0; j < N; j++)
            {
                A[i][j] -= A[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }

    for (int k = N - 1; k > 0; k--)
    {
        for (int i = k - 1; i >= 0; i--)
        {
            temp = A[i][k];

            for (int j = 0; j < N; j++)
            {
                A[i][j] -= A[k][j] * temp;
                E[i][j] -= E[k][j] * temp;
            }
        }
    }

    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            A[i][j] = E[i][j];
}

void transpose()
{
    int M;
    int i,j;
    double **B;
    B=new double *[N];
    if(B==NULL)
    {
       cout<< "Нет ОП \n";
       exit(1);
    }
    for (i=0;i< N;i++)
    {
       B[i]=new double[M];
       if (B[i]==NULL)
       {
           cout<< "Нет ОП \n";
           exit(1);
       }
    }
    for (i=0;i< N;i++)
        for (j=0;j< M;j++)
        B[j][i]=A[i][j];
}

void determinant() {
    int m;
    int i, j, d, k, n;
    double **p;
    p = new double*[m];
    for (i = 0; i<m; i++)
       p[i] = new double[m];
     j = 0; d = 0;
     k = 1; //(-1) в степени i
     n = m - 1;
     if (m<1) cout << "Определитель вычислить невозможно!";
     if (m == 1) {
       d = A[0][0];
     }
     if (m == 2) {
      d = A[0][0] * A[1][1] - (A[1][0] * A[0][1]);
    }
    if (m>2) {
      for (i = 0; i<m; i++) {
        cout << A[i][j] << endl;
        d = d + k * A[i][0];
       k = -k;
      }
    }
};

};

Matrix3f StiffnessUtils::calculateMaterialMatrix(const float &E,
                                                 const float &v) {
  Matrix3f materialMatrix;
  materialMatrix << 1.0f, v, 0.0f, v, 1.0, 0.0f, 0.0f, 0.0f, (1.0f - v) / 2.0f;

  materialMatrix *= E / (1.0f - pow(v, 2.0f));

  return materialMatrix;
}

void StiffnessUtils::calculateLocalStiffnesMatrix(
    const Matrix3f &D, const std::vector<PointF> &pointsOfLocalTriangle,
    const std::vector<int> &indicesOfLocalTriangle,
    std::vector<Triplet<float>> &triplets) {
  Vector3f x;
  x << pointsOfLocalTriangle[0].x, pointsOfLocalTriangle[1].x,
      pointsOfLocalTriangle[2].x;

  Vector3f y;
  y << pointsOfLocalTriangle[0].y, pointsOfLocalTriangle[1].y,
      pointsOfLocalTriangle[2].y;

  Matrix3f C;
  C << Vector3f(1.0f, 1.0f, 1.0f), x, y;

  Matrix3f IC = C.inverse();

  Matrix<float, 3, 6> B;

  for (int i = 0; i < 3; i++) {
    B(0, 2 * i + 0) = IC(1, i);
    B(0, 2 * i + 1) = 0.0f;
    B(1, 2 * i + 0) = 0.0f;
    B(1, 2 * i + 1) = IC(2, i);
    B(2, 2 * i + 0) = IC(2, i);
    B(2, 2 * i + 1) = IC(1, i);
  }

  Matrix<float, 6, 6> K = B.transpose() * D * B * C.determinant() / 2.0f;

  cout << "C" << endl;
  cout << C << endl << endl;

  cout << "B" << endl;
  cout << B << endl << endl;

  cout << "K" << endl;
  cout << K << endl << endl;

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      Eigen::Triplet<float> trplt11(2 * indicesOfLocalTriangle[i] + 0,
                                    2 * indicesOfLocalTriangle[j] + 0,
                                    K(2 * i + 0, 2 * j + 0));
      Eigen::Triplet<float> trplt12(2 * indicesOfLocalTriangle[i] + 0,
                                    2 * indicesOfLocalTriangle[j] + 1,
                                    K(2 * i + 0, 2 * j + 1));
      Eigen::Triplet<float> trplt21(2 * indicesOfLocalTriangle[i] + 1,
                                    2 * indicesOfLocalTriangle[j] + 0,
                                    K(2 * i + 1, 2 * j + 0));
      Eigen::Triplet<float> trplt22(2 * indicesOfLocalTriangle[i] + 1,
                                    2 * indicesOfLocalTriangle[j] + 1,
                                    K(2 * i + 1, 2 * j + 1));

      triplets.push_back(trplt11);
      triplets.push_back(trplt12);
      triplets.push_back(trplt21);
      triplets.push_back(trplt22);
    }
  }
}

SparseMatrix<float> StiffnessUtils::calculateGlobalStiffnesMatrix(
    const int pointsCount, const std::vector<Triplet<float>> &triplets) {
  SparseMatrix<float> globalStiffnesMatrix(2 * pointsCount, 2 * pointsCount);
  globalStiffnesMatrix.setFromTriplets(triplets.begin(), triplets.end());

  return globalStiffnesMatrix;
}

VectorXf StiffnessUtils::applyDisplacments(
    SparseMatrix<float> &stiffnessMatrix, std::vector<bool> &maskedVertices,
    const std::vector<Displacment> &displasments) {
  VectorXf uLocalDisplacments(stiffnessMatrix.cols());
  uLocalDisplacments.setZero();

  VectorXf vLocalDisplacments(stiffnessMatrix.cols());
  vLocalDisplacments.setZero();

  VectorXf globalDisplacments(stiffnessMatrix.cols());
  globalDisplacments.setZero();

  for (const Displacment &displacment : displasments) {
    assert(displacment.pointIndex != -1);

    const int uIndex = 2 * displacment.pointIndex;
    const int vIndex = 2 * displacment.pointIndex + 1;

    float diagonalElement = stiffnessMatrix.coeff(uIndex, uIndex);
    uLocalDisplacments = -stiffnessMatrix.col(uIndex) * displacment.u;
    uLocalDisplacments[uIndex] = diagonalElement * displacment.u;
    uLocalDisplacments[uIndex + 1] = 0.0f;

    for (int rowColIndex = 0; rowColIndex < stiffnessMatrix.cols();
         ++rowColIndex) {
      stiffnessMatrix.coeffRef(rowColIndex, uIndex) = 0.0f;
      stiffnessMatrix.coeffRef(uIndex, rowColIndex) = 0.0f;
    }

    stiffnessMatrix.coeffRef(uIndex, uIndex) = diagonalElement;

    diagonalElement = stiffnessMatrix.coeff(vIndex, vIndex);
    vLocalDisplacments = -stiffnessMatrix.col(vIndex) * displacment.v;
    vLocalDisplacments[vIndex] = diagonalElement * displacment.v;

    for (int rowColIndex = 0; rowColIndex < stiffnessMatrix.cols();
         ++rowColIndex) {
      stiffnessMatrix.coeffRef(rowColIndex, vIndex) = 0.0f;
      stiffnessMatrix.coeffRef(vIndex, rowColIndex) = 0.0f;
    }

    stiffnessMatrix.coeffRef(vIndex, vIndex) = diagonalElement;

    if (!maskedVertices[uIndex])
      globalDisplacments[uIndex] = 0.0f;

    if (!maskedVertices[vIndex])
      globalDisplacments[vIndex] = 0.0f;

    maskedVertices[uIndex] = true;
    maskedVertices[vIndex] = true;

    globalDisplacments += uLocalDisplacments + vLocalDisplacments;
  }

  return globalDisplacments;
}

void StiffnessUtils::applyConstraints(
    SparseMatrix<float> &stiffnessMatrix, VectorXf &displacments,
    const std::vector<Constraints> &constraints) {
  assert(stiffnessMatrix.cols() == displacments.rows());

  for (const Constraints &constraint : constraints) {
    const int uIndex = 2 * constraint.pointIndex;
    const int vIndex = 2 * constraint.pointIndex + 1;

    if (constraint.type == Constraints::Type::None)
      continue;

    if (constraint.type == Constraints::Type::X) {
      for (int rowColIndex = 0; rowColIndex < stiffnessMatrix.cols();
           ++rowColIndex) {
        stiffnessMatrix.coeffRef(rowColIndex, uIndex) = 0.0f;
        stiffnessMatrix.coeffRef(uIndex, rowColIndex) = 0.0f;
      }

      stiffnessMatrix.coeffRef(uIndex, uIndex) = 1.0f;
      displacments(uIndex) = 0.0f;
    }

    if (constraint.type == Constraints::Type::Y) {
      for (int rowColIndex = 0; rowColIndex < stiffnessMatrix.cols();
           ++rowColIndex) {
        stiffnessMatrix.coeffRef(rowColIndex, vIndex) = 0.0f;
        stiffnessMatrix.coeffRef(vIndex, rowColIndex) = 0.0f;
      }

      stiffnessMatrix.coeffRef(vIndex, vIndex) = 1.0f;
      displacments(vIndex) = 0.0f;
    }

    if (constraint.type == Constraints::Type::XY) {
      for (int rowColIndex = 0; rowColIndex < stiffnessMatrix.cols();
           ++rowColIndex) {
        stiffnessMatrix.coeffRef(rowColIndex, uIndex) = 0.0f;
        stiffnessMatrix.coeffRef(uIndex, rowColIndex) = 0.0f;
      }

      stiffnessMatrix.coeffRef(uIndex, uIndex) = 1.0f;
      displacments(uIndex) = 0.0f;

      for (int rowColIndex = 0; rowColIndex < stiffnessMatrix.cols();
           ++rowColIndex) {
        stiffnessMatrix.coeffRef(rowColIndex, vIndex) = 0.0f;
        stiffnessMatrix.coeffRef(vIndex, rowColIndex) = 0.0f;
      }

      stiffnessMatrix.coeffRef(vIndex, vIndex) = 1.0f;
      displacments(vIndex) = 0.0f;
    }
  }
}

VectorXf StiffnessUtils::CholetskySolver(const SparseMatrix<float> &A,
                                         const VectorXf &b) {
  SimplicialLDLT<SparseMatrix<float>> solver(A);
  VectorXf x = solver.solve(b);

  return x;
}

void StiffnessUtils::compute(const float &E, const float &v,
                             QVector<QPointF> &points,
                             const QVector<int> &indices,
                             const QVector<Displacment> &displacments,
                             const QVector<Constraints> &constraints) {
  vector<Eigen::Triplet<float>> triplets;

  Matrix3f D = calculateMaterialMatrix(E, v);

  vector<PointF> convertedPoints;
  for (int pointIndex = 0; pointIndex < points.size(); ++pointIndex)
    convertedPoints.push_back(
        PointF(points[pointIndex].x(), points[pointIndex].y()));

  for (int i = 0; i < indices.size() - 2; i += 3)
    calculateLocalStiffnesMatrix(
        D,
        vector<PointF>{convertedPoints[indices[i]],
                       convertedPoints[indices[i + 1]],
                       convertedPoints[indices[i + 2]]},
        vector<int>{indices[i], indices[i + 1], indices[i + 2]}, triplets);

  SparseMatrix<float> globalStiffnesMatrix =
      calculateGlobalStiffnesMatrix(convertedPoints.size(), triplets);
  VectorXf globalDisplacments;

  cout << "Global Stiffness Matrix" << endl;
  cout << globalStiffnesMatrix << endl;

  vector<bool> maskedVertices(globalStiffnesMatrix.cols(), false);

  vector<Displacment> convertedDisplacment = displacments.toStdVector();
  globalDisplacments = applyDisplacments(globalStiffnesMatrix, maskedVertices,
                                         convertedDisplacment);

  vector<Constraints> convertedConstraints = constraints.toStdVector();
  applyConstraints(globalStiffnesMatrix, globalDisplacments,
                   convertedConstraints);

  VectorXf result = CholetskySolver(globalStiffnesMatrix, globalDisplacments);

  std::cout << "Solution" << std::endl;
  std::cout << result << std::endl;
  std::cout << std::endl;

  for (int pointIndex = 0; pointIndex < points.size(); ++pointIndex) {
    points[pointIndex] += QPointF(result.coeff(pointIndex * 2),
                                  -result.coeff(pointIndex * 2 + 1));
  }
}

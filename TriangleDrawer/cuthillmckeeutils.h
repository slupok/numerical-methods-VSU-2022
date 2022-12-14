#ifndef CUTHILLMCKEEUTILS_H
#define CUTHILLMCKEEUTILS_H

#include <queue>
#include <vector>

#include "triangulationutils.h"

#include <Eigen/Dense>
#include <Eigen/Sparse>

namespace CuthillMckeeUtils {

typedef Eigen::Matrix<bool, Eigen::Dynamic, Eigen::Dynamic> MatrixXb;

MatrixXb adjancecyMatrixFromPointLinks(
    const std::vector<TriangulationUtils::PointWithLinks> &points);

int getBandWidth(const MatrixXb &adjancecyMatrix, const int currentBandWidth);
std::vector<int> getDegrees(const MatrixXb &adjancecyMatrix);
std::vector<int> selectFirstVertice(const MatrixXb &adjancecyMatrix,
                                    int &currentBandWidth,
                                    const std::vector<int> &degrees);
std::vector<int> ReverseAlgorithm(MatrixXb &adjancecyMatrix,
                                  std::vector<int> &degrees,
                                  int currentBandWidth);

} // namespace CuthillMckeeUtils

#endif // CUTHILLMCKEEUTILS_H

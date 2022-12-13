#include "cuthillmckeeutils.h"

#include <iostream>

using namespace std;
using namespace Eigen;
using namespace CuthillMckeeUtils;

int CuthillMckeeUtils::getBandWidth(const MatrixXb &adjancecyMatrix, const int currentBandWidth)
{
    int bandWidth = 0, j = currentBandWidth - 1;
    while (j > 0 && !adjancecyMatrix.coeff(0, j)) j--;
    bandWidth = j + 1;
    if (bandWidth != currentBandWidth)
    {
        for (int i = 1; i < adjancecyMatrix.rows(); ++i)
        {
            j = std::min((int)adjancecyMatrix.rows(), i + currentBandWidth) - 1;
            while (j > i && !adjancecyMatrix.coeff(i, j)) j--;
            if (j - i >= bandWidth) bandWidth = j - i + 1;
            if (bandWidth == currentBandWidth) break;
        }
    }
    return bandWidth;
}

vector<int> CuthillMckeeUtils::getDegrees(const MatrixXb &adjancecyMatrix)
{
    vector<int> degrees(adjancecyMatrix.rows());
    for (int i = 0; i < adjancecyMatrix.rows(); ++i)
    {
        for (int j = 0; j < adjancecyMatrix.cols(); ++j)
        {
            if (adjancecyMatrix.coeff(i, j)) degrees[i]++;
        }
    }
    return degrees;
}

vector<int> CuthillMckeeUtils::selectFirstVertice(const MatrixXb &adjancecyMatrix, int &currentBandWidth, const std::vector<int> &degrees)
{
    int visitedCount, maxDepth = 0;
    queue<int> maxDepthVertices;
    vector<int> visited(adjancecyMatrix.rows());
    vector<int> depths(adjancecyMatrix.rows());
    vector<bool> calculated(adjancecyMatrix.rows());
    vector<int> best(adjancecyMatrix.rows());
    int i, j;
    bool contains = true;
    int newStart = 0;

    do
    {
        visited[0] = newStart;
        calculated[newStart] = true;
        visitedCount = 1;
        for (i = 0; i < adjancecyMatrix.rows(); ++i)
        {
            depths[i] = -1;
        }
        depths[newStart] = 0;
        for (i = 0; i < visited.size() - 1; ++i)
        {
            int v = visited[i];
            int currentDepth = depths[v];
            for (j = v + 1; j <= v + currentBandWidth && j < adjancecyMatrix.rows(); ++j)
            {
                if (adjancecyMatrix.coeff(v, j) && depths[j] < 0)
                {
                    visited[visitedCount] = j;
                    depths[j] = currentDepth + 1;
                    visitedCount++;
                }
            }
            for (j = v - 1; j >= v - currentBandWidth && j > -1; --j)
            {
                if (adjancecyMatrix.coeff(v, j) && depths[j] < 0)
                {
                    visited[visitedCount] = j;
                    depths[j] = currentDepth + 1;
                    visitedCount++;
                }
            }
            if (visitedCount >= adjancecyMatrix.rows()) break;
        }
        i = adjancecyMatrix.rows() - 1;
        if (depths[visited[i]] >= maxDepth)
        {
            if (depths[visited[i]] > maxDepth)
            {
                maxDepth = depths[visited[i]];
                best = visited;

                while (!maxDepthVertices.empty())
                    maxDepthVertices.pop();
            }
            int minDegree = degrees[visited[i]];
            int iWithMinDegree = i;
            i--;
            while (depths[visited[i]] == maxDepth)
            {
                int degree = degrees[visited[i]];
                if (degree < minDegree)
                {
                    minDegree = degree;
                    iWithMinDegree = i;
                }
                i--;
            }
            if (!calculated[visited[iWithMinDegree]])
            {
                maxDepthVertices.push(visited[iWithMinDegree]);
            }
            i = iWithMinDegree - 1;
            while (depths[visited[i]] == maxDepth)
            {
                if (degrees[visited[i]] == minDegree)
                {
                    if (!calculated[visited[i]])
                    {
                        maxDepthVertices.push(visited[i]);
                    }
                }
                i--;
            }
        }

        contains = !maxDepthVertices.empty();

        if (contains) {
            newStart = maxDepthVertices.front();
            maxDepthVertices.pop();
        }

    } while (contains);
    return best;
}

vector<int> CuthillMckeeUtils::ReverseAlgorithm(MatrixXb &adjancecyMatrix, std::vector<int> &degrees, int currentBandWidth)
{
    vector<int> order = selectFirstVertice(adjancecyMatrix, currentBandWidth, degrees);
    int center = adjancecyMatrix.rows() / 2, i, j, temp;
    for (i = 0; i < center; ++i)
    {
        temp = order[i];
        order[i] = order[adjancecyMatrix.rows() - i - 1];
        order[adjancecyMatrix.rows() - i - 1] = temp;
    }
    // Changing matrix and swapping degrees
    vector<int> newDegrees(adjancecyMatrix.rows());
    MatrixXb newMatrix(adjancecyMatrix.rows(), adjancecyMatrix.rows());
    for (i = 0; i < adjancecyMatrix.rows(); ++i)
    {
        newDegrees[i] = degrees[order[i]];
        for (j = 0; j < adjancecyMatrix.rows(); ++j)
        {
            newMatrix.coeffRef(i, j) = adjancecyMatrix.coeff(order[i], order[j]);
        }
    }
    adjancecyMatrix = newMatrix;
    degrees = newDegrees;
    return order;
}

MatrixXb CuthillMckeeUtils::adjancecyMatrixFromPointLinks(const std::vector<TriangulationUtils::PointWithLinks> &points)
{
    MatrixXb adjancecyMatrix(points.size(), points.size());

    for (int rowIndex = 0; rowIndex < adjancecyMatrix.rows(); ++rowIndex)
        for (int colIndex = 0; colIndex < adjancecyMatrix.cols(); ++colIndex)
            adjancecyMatrix.coeffRef(rowIndex, colIndex) = false;

    for (int rowIndex = 0; rowIndex < adjancecyMatrix.rows(); ++rowIndex)
        for (int linkIndex = 0; linkIndex < points[rowIndex].links.size(); ++linkIndex)
            adjancecyMatrix.coeffRef(rowIndex, points[rowIndex].links[linkIndex]) = true;

    return adjancecyMatrix;
}

#ifndef VIEWPORT_H
#define VIEWPORT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QDebug>

class Viewport : public QGraphicsView
{
    Q_OBJECT
public:
    explicit Viewport(QWidget *parent = nullptr);

    void setPoints(const QVector<QPointF> &points);
    void setTriangleIndices(const QVector<int> &triangleIndices);

    void update();

    int selectedPoint() const;

signals:
    void onPointSelected(const int pointIndex);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

private:
    void drawPoints();
    void drawTriangles();

    QGraphicsScene *m_scene = nullptr;

    const float m_radiusSearch = 0.1f;
    int m_pointSelected = -1;

    const int m_pointSize = 2;
    const float m_scaleFactor = 30.0f;

    const QColor m_pointColor = Qt::green;
    const QColor m_selectedPointColor = Qt::red;
    const QColor m_lineColor = Qt::blue;

    QVector<QPointF> m_points;
    QVector<int> m_triangleIndices;

    float m_currentZoom = 0.0f;
    const float m_zoomFactor = 0.25f;
};

#endif // VIEWPORT_H

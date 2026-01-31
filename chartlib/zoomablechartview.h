#ifndef ZOOMABLECHARTVIEW_H
#define ZOOMABLECHARTVIEW_H

#include <QtCharts/QChartView>
#include <QtCharts/QValueAxis>
#include <QPoint>

QT_USE_NAMESPACE

class ZoomableChartView : public QChartView
{
    Q_OBJECT

public:
    explicit ZoomableChartView(QChart *chart, QWidget *parent = nullptr);

protected:
    void wheelEvent(QWheelEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPoint m_lastMousePos;
    bool m_isPanning = false;
    
    QValueAxis* getAxisAtPosition(const QPoint& pos, Qt::Orientation& axis);
};

#endif // ZOOMABLECHARTVIEW_H

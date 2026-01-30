#include "zoomablechartview.h"
#include <QWheelEvent>
#include <QMouseEvent>
#include <QtCharts/QChart>
#include <QtCharts/QAbstractAxis>

ZoomableChartView::ZoomableChartView(QChart *chart, QWidget *parent)
    : QChartView(chart, parent)
{
}

QValueAxis* ZoomableChartView::getAxisAtPosition(const QPoint& pos, Qt::Orientation& axis)
{
    if (!chart()) return nullptr;
    if (chart()->series().isEmpty()) return nullptr;

    // Pobierz granice obszaru wykresu
    QRect plotArea = chart()->plotArea().toRect();

    // Margines do detekcji bliskości osi (w pikselach)
    const int margin = 50;

    // Jeśli kursor jest blisko lewej krawędzi (oś Y)
    if (pos.x() < plotArea.left() + margin) {
        axis = Qt::Vertical;
        // Zwróć lewą oś Y
        auto axes = chart()->axes(Qt::Vertical, chart()->series().first());
        if (!axes.isEmpty()) {
            return qobject_cast<QValueAxis*>(axes.first());
        }
    }
    // Jeśli kursor jest blisko dolnej krawędzi (oś X)
    else if (pos.y() > plotArea.bottom() - margin) {
        axis = Qt::Horizontal;
        // Zwróć dolną oś X
        auto axes = chart()->axes(Qt::Horizontal, chart()->series().first());
        if (!axes.isEmpty()) {
            return qobject_cast<QValueAxis*>(axes.first());
        }
    }

    return nullptr;
}

void ZoomableChartView::wheelEvent(QWheelEvent *event)
{
    if (!chart()) {
        event->accept();
        return;
    }

    Qt::Orientation axis;
    QPoint pos = QPoint(event->position().x(), event->position().y());
    QValueAxis* targetAxis = getAxisAtPosition(pos, axis);

    qreal factor = event->angleDelta().y() > 0 ? 1.2 : 0.8;

    if (targetAxis) {
        // Zoom selektywny dla wyznaczonej osi
        qreal min = targetAxis->min();
        qreal max = targetAxis->max();
        qreal range = max - min;
        qreal center = (min + max) / 2.0;

        qreal newRange = range / factor;
        qreal newMin = center - newRange / 2.0;
        qreal newMax = center + newRange / 2.0;

        targetAxis->setRange(newMin, newMax);
    } else {
        // Jeśli kursor nie nad osią, robić ogólny zoom (obie osie)
        chart()->zoom(factor);
    }

    event->accept();
}

void ZoomableChartView::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton) {
        m_lastMousePos = event->pos();
        m_isPanning = true;
        event->accept();
    } else {
        QChartView::mousePressEvent(event);
    }
}

void ZoomableChartView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_isPanning && chart()) {
        // Oblicz różnicę pozycji myszy
        QPoint delta = event->pos() - m_lastMousePos;
        
        // Przesuwaj wykres
        chart()->scroll(-delta.x(), delta.y());
        
        // Zaktualizuj ostatnią pozycję
        m_lastMousePos = event->pos();
        event->accept();
    } else {
        QChartView::mouseMoveEvent(event);
    }
}

void ZoomableChartView::mouseReleaseEvent(QMouseEvent *event)
{
    if ((event->button() == Qt::MiddleButton || event->button() == Qt::LeftButton) && m_isPanning) {
        m_isPanning = false;
        event->accept();
    } else {
        QChartView::mouseReleaseEvent(event);
    }
}

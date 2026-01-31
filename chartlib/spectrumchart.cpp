#include "spectrumchart.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QtCharts/QChart>
#include <QtCharts/QAbstractSeries>
#include <limits>

SpectrumChartWidget::SpectrumChartWidget(QWidget* parent) : QWidget(parent)
{
    m_chart = new QChart();
    m_chart->setTitle("Energy spectrum");
    m_chart->legend()->setVisible(true);

    m_axisX = new QValueAxis();
    m_axisX->setTitleText("Energy");
    m_axisX->setLabelsVisible(true);

    m_axisY = new QValueAxis();
    m_axisY->setTitleText("Normalized Counts");
    m_axisY->setLabelsVisible(true);

    m_chart->addAxis(m_axisX, Qt::AlignBottom);
    m_chart->addAxis(m_axisY, Qt::AlignLeft);

    m_chartView = new ZoomableChartView(m_chart, this);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_chartView);
}

QLineSeries* SpectrumChartWidget::addSeries(const QString& name, const QVector<QPointF>& points)
{
    if (points.isEmpty()) {
        return nullptr;
    }

    auto* series = new QLineSeries(m_chart);
    series->setName(name);
    series->append(points);

    m_chart->addSeries(series);
    series->attachAxis(m_axisX);
    series->attachAxis(m_axisY);

    ensureAxisRange(points);
    return series;
}

void SpectrumChartWidget::removeSeries(QLineSeries* series)
{
    if (!series) {
        return;
    }

    m_chart->removeSeries(series);
    series->deleteLater();
    recalcAxisRanges();
}

void SpectrumChartWidget::clear()
{
    const auto seriesList = m_chart->series();
    for (QAbstractSeries* s : seriesList) {
        m_chart->removeSeries(s);
        s->deleteLater();
    }
    m_hasData = false;
    m_axisX->setRange(0.0, 1.0);
    m_axisY->setRange(0.0, 1.0);
}

void SpectrumChartWidget::ensureAxisRange(const QVector<QPointF>& points)
{
    if (points.isEmpty()) {
        return;
    }

    double minX = points.first().x();
    double maxX = points.first().x();
    double minY = points.first().y();
    double maxY = points.first().y();

    for (const auto& p : points) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }

    if (!m_hasData) {
        const double paddingX = qFuzzyIsNull(maxX - minX) ? 1.0 : (maxX - minX) * 0.1;
        const double paddedMinX = qMax(0.0, minX - paddingX);
        const double paddedMaxX = maxX + paddingX;
        const double paddingY = qFuzzyIsNull(maxY) ? 0.1 : maxY * 0.1;
        const double paddedMaxY = maxY + paddingY;

        m_axisX->setRange(paddedMinX, paddedMaxX);
        m_axisY->setRange(0.0, paddedMaxY);
        m_hasData = true;
        return;
    }

    const double newMinX = qMax(0.0, qMin(m_axisX->min(), minX));
    const double newMaxX = qMax(m_axisX->max(), maxX);
    const double paddingX = qFuzzyIsNull(newMaxX - newMinX) ? 1.0 : (newMaxX - newMinX) * 0.1;
    m_axisX->setRange(qMax(0.0, newMinX - paddingX), newMaxX + paddingX);

    const double newMaxY = qMax(m_axisY->max(), maxY);
    const double paddingY = qFuzzyIsNull(newMaxY) ? 0.1 : newMaxY * 0.1;
    m_axisY->setRange(qMax(0.0, qMin(m_axisY->min(), minY)), newMaxY + paddingY);
}

void SpectrumChartWidget::recalcAxisRanges()
{
    const auto allSeries = m_chart->series();
    if (allSeries.isEmpty()) {
        m_hasData = false;
        m_axisX->setRange(0.0, 1.0);
        m_axisY->setRange(0.0, 1.0);
        return;
    }

    double minX = std::numeric_limits<double>::max();
    double maxX = std::numeric_limits<double>::lowest();
    double minY = std::numeric_limits<double>::max();
    double maxY = std::numeric_limits<double>::lowest();

    for (QAbstractSeries* series : allSeries) {
        auto* line = qobject_cast<QLineSeries*>(series);
        if (!line) continue;

        const auto pts = line->points();
        for (const auto& p : pts) {
            minX = qMin(minX, p.x());
            maxX = qMax(maxX, p.x());
            minY = qMin(minY, p.y());
            maxY = qMax(maxY, p.y());
        }
    }

    if (maxX < minX || maxY < minY) {
        m_axisX->setRange(0.0, 1.0);
        m_axisY->setRange(0.0, 1.0);
        m_hasData = false;
        return;
    }

    const double paddingX = qFuzzyIsNull(maxX - minX) ? 1.0 : (maxX - minX) * 0.1;
    const double paddingY = qFuzzyIsNull(maxY) ? 0.1 : maxY * 0.1;

    m_axisX->setRange(qMax(0.0, minX - paddingX), maxX + paddingX);
    m_axisY->setRange(qMax(0.0, minY), maxY + paddingY);
    m_hasData = true;
}

void SpectrumChartWidget::centerOnSeries(QLineSeries* series)
{
    if (!series) return;
    const auto pts = series->points();
    if (pts.isEmpty()) return;

    double minX = pts.first().x();
    double maxX = pts.first().x();
    double minY = pts.first().y();
    double maxY = pts.first().y();

    for (const auto& p : pts) {
        minX = qMin(minX, p.x());
        maxX = qMax(maxX, p.x());
        minY = qMin(minY, p.y());
        maxY = qMax(maxY, p.y());
    }

    const double paddingX = qFuzzyIsNull(maxX - minX) ? 1.0 : (maxX - minX) * 0.1;
    const double paddingY = qFuzzyIsNull(maxY) ? 0.1 : maxY * 0.1;

    m_axisX->setRange(minX - paddingX, maxX + paddingX);
    m_axisY->setRange(qMin(0.0, minY), maxY + paddingY);
    m_hasData = true;
}

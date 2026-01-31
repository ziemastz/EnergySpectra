#ifndef SPECTRUMCHART_H
#define SPECTRUMCHART_H

#include <QWidget>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

#include "zoomablechartview.h"

class SpectrumChartWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SpectrumChartWidget(QWidget* parent = nullptr);

    QLineSeries* addSeries(const QString& name, const QVector<QPointF>& points);
    void removeSeries(QLineSeries* series);
    void clear();
    void centerOnSeries(QLineSeries* series);

    QChart* chart() const { return m_chart; }
    ZoomableChartView* view() const { return m_chartView; }

private:
    void ensureAxisRange(const QVector<QPointF>& points);
    void recalcAxisRanges();

    QChart* m_chart = nullptr;
    ZoomableChartView* m_chartView = nullptr;
    QValueAxis* m_axisX = nullptr;
    QValueAxis* m_axisY = nullptr;
    bool m_hasData = false;
};

#endif // SPECTRUMCHART_H

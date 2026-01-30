#ifndef SPECTRUMDTOS_H
#define SPECTRUMDTOS_H

#include <QString>
#include <QVector>
#include <QPointF>

struct SpectrumStats {
    double entries = 0;
    double mean = 0;
    double minE = 0;
    double maxE = 0;
    int eventCount = 0;
    int zeroCount = 0;
};

struct SpectrumResult {
    QString sourcePath;
    QString label;
    QVector<QPointF> points;
    SpectrumStats stats;
};

#endif // SPECTRUMDTOS_H

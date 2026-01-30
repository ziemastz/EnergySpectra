#include "spectrumservice.h"
#include <QFile>
#include <QTextStream>
#include <QtMath>
#include <QRegularExpression>
#include <QSet>

SpectrumService::SpectrumService(QObject* parent) : QObject(parent) {}

struct EnergyLineResult {
    int eventId = -1;
    QVector<double> energies;
};

static EnergyLineResult parseEnergyLine(const QString& line)
{
    EnergyLineResult result;

    QString trimmed = line.trimmed();
    if (trimmed.isEmpty()) return result;
    if (trimmed.startsWith('#')) return result;

    // Rozbij po dowolnej liczbie białych znaków (spacja, tab)
    const QStringList tokens = trimmed.split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);

    // Minimum: 2 kolumny (event id + energia)
    if (tokens.size() < 2) return result;

    // tokens[0] = numer eventu
    bool ok = false;
    result.eventId = tokens[0].toInt(&ok);
    if (!ok) result.eventId = -1;

    for (int i = 1; i < tokens.size(); ++i) {
        bool ok = false;
        double e = tokens[i].toDouble(&ok);
        if (ok) {
            result.energies.push_back(e);
        }
    }

    return result;
}

void SpectrumService::LoadSpectrum(const QString &filePath, double binWidth, const QString &label)
{
    if (binWidth <= 0) {
        emit spectrumError(filePath, "binWidth must be > 0");
        return;
    }

    QFile f(filePath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit spectrumError(filePath, "Cannot open file");
        return;
    }

    QVector<double> energies;
    energies.reserve(200000);
    QSet<int> uniqueEvents;
    int zeroCount = 0;

    QTextStream in(&f);
    while (!in.atEnd()) {
        const QString line = in.readLine();
        const EnergyLineResult parsed = parseEnergyLine(line);
        
        // Oddziel zera od reszty
        for (double e : parsed.energies) {
            if (e == 0.0) {
                zeroCount++;
            } else {
                energies.push_back(e);
            }
        }
        
        if (parsed.eventId >= 0) {
            uniqueEvents.insert(parsed.eventId);
        }
    }

    if (energies.isEmpty()) {
        emit spectrumError(filePath, "No energies parsed from file");
        return;
    }

    // Min/Max
    double minE = energies[0], maxE = energies[0];
    double sum = 0, sum2 = 0;
    for (double e : energies) {
        minE = qMin(minE, e);
        maxE = qMax(maxE, e);
        sum += e;
        sum2 += e * e;
    }

    const double n = energies.size();
    const double mean = sum / n;

    // Histogram bins
    const int binCount = qMax(1, int(qCeil((maxE - minE) / binWidth)));
    QVector<int> bins(binCount);
    bins.fill(0);

    for (double e : energies) {
        int idx = int((e - minE) / binWidth);
        if (idx < 0) idx = 0;
        if (idx >= binCount) idx = binCount - 1;
        bins[idx]++;
    }

    // Wybierz dzielnik: gdy brak ID, normalizuj po liczbie wpisów
    const int eventCount = uniqueEvents.isEmpty() ? energies.size() : uniqueEvents.size();

    // Zbuduj punkty do wykresu (normalizacja po liczbie eventów)
    QVector<QPointF> points;
    points.reserve(binCount);
    for (int i = 0; i < binCount; ++i) {
        const double x = minE + (i + 0.5) * binWidth;
        const double normalized = static_cast<double>(bins[i]) / static_cast<double>(eventCount);
        points.append(QPointF(x, normalized));
    }

    SpectrumResult result;
    result.sourcePath = filePath;
    result.label = label;
    result.points = std::move(points);
    result.stats = SpectrumStats{ n, mean, minE, maxE, eventCount, zeroCount };

    emit spectrumReady(std::move(result));
}

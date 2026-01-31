#ifndef SPECTRUMSERVICE_H
#define SPECTRUMSERVICE_H

#include <QObject>
#include "SpectrumDtos.h"

class SpectrumService : public QObject
{
    Q_OBJECT
public:
    explicit SpectrumService(QObject* parent = nullptr);

public slots:
    void LoadSpectrum(const QString& filename, double binWidth, const QString& label);

signals:
    void spectrumReady(SpectrumResult result);
    void spectrumError(QString filePath, QString message);
};

#endif // SPECTRUMSERVICE_H

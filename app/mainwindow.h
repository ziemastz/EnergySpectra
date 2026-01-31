#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QMap>
#include <QThread>
#include <QtCharts/QLineSeries>
#include "spectrumservice.h"
#include "spectrumchart.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_add_pushButton_clicked();
    void onSpectrumReady(const SpectrumResult& result);
    void onSpectrumError(const QString& path, const QString& message);
    void on_remove_pushButton_clicked();
    void onExportDataClicked();
    void onExportImageClicked();
    void onTableCellDoubleClicked(int row, int column);

private:
    Ui::MainWindow *ui;
    SpectrumService* m_service = nullptr;
    QThread* m_serviceThread = nullptr;
    SpectrumChartWidget* m_chartWidget = nullptr;
    QMap<int, QLineSeries*> m_seriesByRow;
};
#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QMap>
#include <QThread>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QLineSeries>
#include "SpectrumService.h"
#include "zoomablechartview.h"

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
    void onSeriesVisibilityChanged(int row, int column);

    void on_remove_pushButton_clicked();

private:
    Ui::MainWindow *ui;
    SpectrumService* m_service = nullptr;
    QThread* m_serviceThread = nullptr;
    QChart* m_chart = nullptr;
    ZoomableChartView* m_chartView = nullptr;
    QMap<int, QLineSeries*> m_seriesByRow;
};
#endif // MAINWINDOW_H

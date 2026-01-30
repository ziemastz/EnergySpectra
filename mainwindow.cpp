#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QCheckBox>
#include <QFileInfo>
#include <QApplication>
#include <QStatusBar>
#include <QThread>
#include <QMetaType>
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>
#include <QVBoxLayout>
#include <QPainter>
#include <QTableWidgetItem>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    qRegisterMetaType<SpectrumResult>("SpectrumResult");

    m_serviceThread = new QThread(this);
    m_service = new SpectrumService();
    m_service->moveToThread(m_serviceThread);
    connect(m_serviceThread, &QThread::finished, m_service, &QObject::deleteLater);
    connect(m_service, &SpectrumService::spectrumReady, this, &MainWindow::onSpectrumReady);
    connect(m_service, &SpectrumService::spectrumError, this, &MainWindow::onSpectrumError);
    m_serviceThread->start();

    m_chart = new QChart();
    m_chart->setTitle("Energy spectrum");
    m_chart->legend()->setVisible(true);

    auto* axisX = new QValueAxis();
    axisX->setTitleText("Energy");
    axisX->setLabelsVisible(true);
    
    auto* axisY = new QValueAxis();
    axisY->setTitleText("Normalized Counts");
    axisY->setLabelsVisible(true);

    m_chart->addAxis(axisX, Qt::AlignBottom);
    m_chart->addAxis(axisY, Qt::AlignLeft);

    m_chartView = new ZoomableChartView(m_chart, ui->chart_widget);
    m_chartView->setRenderHint(QPainter::Antialiasing);

    auto* layout = new QVBoxLayout(ui->chart_widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_chartView);
}

MainWindow::~MainWindow()
{
    if (m_serviceThread) {
        m_serviceThread->quit();
        m_serviceThread->wait();
    }
    delete ui;
}

void MainWindow::on_add_pushButton_clicked()
{
    QString filename = QFileDialog::getOpenFileName(
        this,
        tr("Select file with energies"),
        QString(),
        tr("Energy file (*.tab *.dat *.txt);;All files (*.*)"));

    if (filename.isEmpty()) {
        return;
    }

    const double binWidth = 0.1;                 // np. z UI (spinbox)
    const QString label = QFileInfo(filename).baseName();

    // Pokaż wskaźnik ładowania i zablokuj przycisk, przywrócenie nastąpi w onSpectrumReady/onSpectrumError
    QApplication::setOverrideCursor(Qt::WaitCursor);
    ui->add_pushButton->setEnabled(false);
    if (ui->statusbar) {
        ui->statusbar->showMessage(tr("Loading spectrum..."));
        ui->statusbar->setVisible(true);
    }

    // Wywołaj LoadSpectrum w wątku serwisu (queued invocation)
    QMetaObject::invokeMethod(m_service, "LoadSpectrum", Qt::QueuedConnection,
                              Q_ARG(QString, filename), Q_ARG(double, binWidth), Q_ARG(QString, label));
}

void MainWindow::onSpectrumReady(const SpectrumResult& result)
{
    auto* series = new QLineSeries();
    series->setName(result.label);

    for (const auto& p : result.points)
        series->append(p);

    m_chart->addSeries(series);

    // Podepnij osie (zakładamy, że już istnieją)
    auto* axisX = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Horizontal).first());
    auto* axisY = qobject_cast<QValueAxis*>(m_chart->axes(Qt::Vertical).first());
    
    if (axisX && axisY) {
        series->attachAxis(axisX);
        series->attachAxis(axisY);
        
        // Ustaw zakresy osi na podstawie danych
        double minX = result.stats.minE;
        double maxX = result.stats.maxE;
        double padding = (maxX - minX) * 0.1;  // 10% paddingu
        
        axisX->setRange(minX - padding, maxX + padding);
        
        // Dla osi Y, znajdź maksymalną liczbę zliczonych energii
        double maxY = 0;
        for (const auto& point : result.points) {
            if (point.y() > maxY) {
                maxY = point.y();
            }
        }
        axisY->setRange(0, maxY * 1.1);  // 10% buforu na górze
    }

    // Dodaj wiersz do tabeli
    // Używaj rowCount() zamiast m_rowCount aby prawidłowo działało po usunięciu wierszy
    const int row = ui->files_tableWidget->rowCount();

    // Upewnij się, że tabela ma wystarczającą liczbę kolumn (9 kolumn: checkbox + 8 danych)
    if (ui->files_tableWidget->columnCount() < 9) {
        ui->files_tableWidget->setColumnCount(9);
    }

    // Blokuj sygnały aby uniknąć triggerowania podczas ustawiania itemów
    ui->files_tableWidget->blockSignals(true);

    ui->files_tableWidget->insertRow(row);

    // Kolumna 0: checkbox dla widoczności
    auto* checkBox = new QCheckBox();
    checkBox->setChecked(true);
    ui->files_tableWidget->setCellWidget(row, 0, checkBox);

    // Podłącz sygnał checkboxa do slotu (użyjemy toggled(bool))
    // WAŻNE: przechowujemy seria zamiast row, żeby checkbox działał nawet po usunięciu innych wierszy
    connect(checkBox, &QCheckBox::toggled, this, [series](bool checked) {
        if (series) {
            series->setVisible(checked);
        }
    });

    // Pozostałe kolumny ze statystykami
    ui->files_tableWidget->setItem(row, 1, new QTableWidgetItem(QString::number(row + 1)));
    ui->files_tableWidget->setItem(row, 2, new QTableWidgetItem(result.label));
    ui->files_tableWidget->setItem(row, 3, new QTableWidgetItem(QString::number(result.stats.eventCount)));
    ui->files_tableWidget->setItem(row, 4, new QTableWidgetItem(QString::number((int)result.stats.entries)));
    ui->files_tableWidget->setItem(row, 5, new QTableWidgetItem(QString::number(result.stats.maxE, 'f', 2)));
    ui->files_tableWidget->setItem(row, 6, new QTableWidgetItem(QString::number(result.stats.minE, 'f', 2)));
    ui->files_tableWidget->setItem(row, 7, new QTableWidgetItem(QString::number(result.stats.mean, 'f', 2)));
    ui->files_tableWidget->setItem(row, 8, new QTableWidgetItem(QString::number(result.stats.zeroCount)));

    // Przechowaj serię
    m_seriesByRow[row] = series;
    
    // Odblouj sygnały
    ui->files_tableWidget->blockSignals(false);
    
    // Dopasuj szerokość kolumn
    ui->files_tableWidget->resizeColumnsToContents();

    // Przywróć UI po zakończeniu ładowania
    QApplication::restoreOverrideCursor();
    ui->add_pushButton->setEnabled(true);
    if (ui->statusbar) ui->statusbar->showMessage(tr("Load complete"), 2000);
}

void MainWindow::onSpectrumError(const QString& path, const QString& message)
{
    // Przywróć UI
    QApplication::restoreOverrideCursor();
    ui->add_pushButton->setEnabled(true);
    if (ui->statusbar) ui->statusbar->clearMessage();

    QMessageBox::warning(this, "Błąd", path + "\n" + message);
}

void MainWindow::onSeriesVisibilityChanged(int row, int column)
{
    Q_UNUSED(column);
    if (!m_seriesByRow.contains(row)) {
        return;
    }

    auto* cb = qobject_cast<QCheckBox*>(ui->files_tableWidget->cellWidget(row, 0));
    if (!cb) return;

    QLineSeries* series = m_seriesByRow[row];
    series->setVisible(cb->isChecked());
}



void MainWindow::on_remove_pushButton_clicked()
{
    // Pobierz wybrany wiersz z tabeli
    int selectedRow = ui->files_tableWidget->currentRow();
    
    // Jeśli żaden wiersz nie zaznaczony, nic nie rób
    if (selectedRow < 0) {
        QMessageBox::warning(this, tr("Warning"), tr("Select a spectrum to remove"));
        return;
    }

    // Usuń serię z wykresu
    if (m_seriesByRow.contains(selectedRow)) {
        QLineSeries* series = m_seriesByRow[selectedRow];
        m_chart->removeSeries(series);
        delete series;
        m_seriesByRow.remove(selectedRow);
    }

    // Usuń wiersz z tabeli
    ui->files_tableWidget->removeRow(selectedRow);

    // Przelicz nowe indeksy dla pozostałych wierszy
    QMap<int, QLineSeries*> newSeriesByRow;
    for (auto it = m_seriesByRow.begin(); it != m_seriesByRow.end(); ++it) {
        int oldRow = it.key();
        int newRow = oldRow > selectedRow ? oldRow - 1 : oldRow;
        newSeriesByRow[newRow] = it.value();
    }
    m_seriesByRow = newSeriesByRow;

    // Zaktualizuj numerację w kolumnie Id
    for (int row = 0; row < ui->files_tableWidget->rowCount(); ++row) {
        auto* idItem = ui->files_tableWidget->item(row, 1);
        if (!idItem) {
            idItem = new QTableWidgetItem();
            ui->files_tableWidget->setItem(row, 1, idItem);
        }
        idItem->setText(QString::number(row + 1));
    }
    
    // Odśwież rozmiar kolumn
    ui->files_tableWidget->resizeColumnsToContents();
}


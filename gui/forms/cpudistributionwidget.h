#ifndef CPUDISTRIBUTIONWIDGET_H
#define CPUDISTRIBUTIONWIDGET_H

#include <QWidget>
#include <QtCharts>

#include "../../datasources/BaseDataSource.h"
#include "../../types/series/TimeSeries.h"
#include "../../types/AdvancedChart.h"

namespace Ui {
class CPUDistributionWidget;
}

//TODO: выбор источника
class CPUDistributionWidget : public QWidget
{
    Q_OBJECT

public:
    CPUDistributionWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~CPUDistributionWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::CPUDistributionWidget *ui;
    QTimer *timer;
    QVector<TimeSeries<QLineSeries>*> series;
    BaseDataSource *data_source;
    AdvancedChart *chart;
    QChartView *chart_view;
    CPUDistributionSource source;

private slots:
    void onTimerTimeout();
    void onSourceChanged(const QString &_source);
};

#endif // CPUDISTRIBUTIONWIDGET_H

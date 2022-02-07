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
    QChartView *chart_view;

private slots:
    void onTimerTimeout();
};

#endif // CPUDISTRIBUTIONWIDGET_H

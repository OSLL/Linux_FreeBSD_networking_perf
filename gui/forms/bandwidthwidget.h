#ifndef BANDWIDTHWIDGET_H
#define BANDWIDTHWIDGET_H

#include <QWidget>

#include "../../utils/default_args.h"
#include "../../types/BandwidthResult.h"
#include "../../types/enums/BandwidthUnits.h"
#include "../../types/enums/UnitsPrefixes.h"
#include "../../types/BandwidthChart.h"
#include "../../types/series/CounterSeries.h"

namespace Ui {
class BandwidthWidget;
}

class BandwidthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BandwidthWidget(const QVector<BandwidthResult>& _result, const QVector<BandwidthUnits>& available_units,
                             QWidget *parent = nullptr);
    ~BandwidthWidget();

protected:
    void changeEvent(QEvent *e);

    FuncSeries<CounterSeries<QLineSeries>, qreal, BandwidthResult> *bandwidth_series;
    BandwidthChart *chart;
    QChartView chart_view;
    const QVector<BandwidthResult> result;

private:
    Ui::BandwidthWidget *ui;
    void recreateChart();

public slots:
    void prefixChanged(const QString &text);
    void unitChanged(const QString &text);
};

#endif // BANDWIDTHWIDGET_H

#ifndef SENDBANDWIDTHWIDGET_H
#define SENDBANDWIDTHWIDGET_H

#include <QWidget>
#include <QtCharts>

#include "../../datasources/BaseDataSource.h"
#include "../../types/BandwidthChart.h"
#include "startstopwidget.h"
#include "../threads/BandwidthSenderThread.h"

namespace Ui {
class SendBandwidthWidget;
}

class SendBandwidthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendBandwidthWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~SendBandwidthWidget() override;

protected:
    void changeEvent(QEvent *e) override;

    StartStopWidget start_stop;
    QChartView chart_view;
    BaseDataSource *data_source;

    BandwidthSeries *bandwidth_series;
    BandwidthChart *chart;
    BandwidthSenderThread *sender_thread;

protected slots:

    void onStartClicked();
    void onStopClicked();
    void onBandwidth(BandwidthResult result);
    void onFinished();

private:
    Ui::SendBandwidthWidget *ui;
};

#endif // SENDBANDWIDTHWIDGET_H

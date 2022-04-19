#ifndef LINUXPKTGENWIDGET_H
#define LINUXPKTGENWIDGET_H

#include <QWidget>
#include "../../datasources/BaseDataSource.h"
#include "../../types/BandwidthChart.h"
#include "../../datasources/bandwidth/LinuxPktgen.h"
#include "../../utils/default_args.h"
#include "startstopwidget.h"
#include "../threads/LinuxPktgenThread.h"

namespace Ui {
class LinuxPktgenWidget;
}

class LinuxPktgenWidget : public QWidget
{
    Q_OBJECT

public:
    LinuxPktgenWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~LinuxPktgenWidget();

protected:
    void changeEvent(QEvent *e);

protected slots:
    void onStart();
    void onStop();
    void onBandwidthResult(BandwidthResult result);
    void onThreadFinished();

private:
    Ui::LinuxPktgenWidget *ui;
    BaseDataSource *data_source;
    QChartView chart_view;
    BandwidthSeries *bandwidth_series;
    BandwidthChart *chart;
    StartStopWidget *start_stop;
    LinuxPktgenThread *thread;
};

#endif // LINUXPKTGENWIDGET_H

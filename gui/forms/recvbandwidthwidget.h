#ifndef RECVBANDWIDTHWIDGET_H
#define RECVBANDWIDTHWIDGET_H

#include <QWidget>
#include <QtCharts>
#include <QMessageBox>

#include "../../../utils/sockets.h"
#include "../../../utils/default_args.h"
#include "startstopwidget.h"
#include "../../datasources/BaseDataSource.h"
#include "../../types/AdvancedChart.h"
#include "../../types/series/TimeSeries.h"
#include "../threads/BandwidthReceiverThread.h"
#include "../../types/enums/UnitsPrefixes.h"
#include "../../types/enums/BandwidthUnits.h"
#include "../../types/BandwidthChart.h"

namespace Ui {
class RecvBandwidthWidget;
}

class RecvBandwidthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecvBandwidthWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~RecvBandwidthWidget();

protected:
    void changeEvent(QEvent *e);

    StartStopWidget start_stop;
    QChartView chart_view;
    BaseDataSource *data_source;

    BandwidthSeries *bandwidth_series;
    BandwidthChart *chart;

protected slots:

    void onStartClicked();
    void onStopClicked();
    void onBandwidth(BandwidthResult result);
    void onFinished();

private:
    Ui::RecvBandwidthWidget *ui;
    BandwidthReceiverThread *receiver_thread;
};

#endif // RECVBANDWIDTHWIDGET_H

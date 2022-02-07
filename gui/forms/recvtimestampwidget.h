#ifndef RECVTIMESTAMPWIDGET_H
#define RECVTIMESTAMPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QtCharts>

#include "../../datasources/BaseDataSource.h"
#include "../../types/series/TimeSeries.h"
#include "../../types/series/FuncSeries.h"
#include "../../types/TimestampsChart.h"
#include "../threads/TimestampsReceiverThread.h"
#include "../../utils/default_args.h"

#include "startstopwidget.h"

namespace Ui {
class RecvTimestampWidget;
}

using namespace std::placeholders;

class RecvTimestampWidget : public QWidget
{
    Q_OBJECT

public:
    RecvTimestampWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~RecvTimestampWidget();

protected:
    void changeEvent(QEvent *e);
    void recreateChart(quint64 packets_count, bool is_us);

private:
    Ui::RecvTimestampWidget *ui;
    BaseDataSource *data_source;

    QChartView chart_view;
    TimestampsChart *chart;
    StartStopWidget start_stop;

    TimestampSeries *software_series;
    TimestampSeries *hardware_series;
    TimestampSeries *in_call_series;
    TimestampSeries *total_series;

    TimestampsReceiverThread *receiver_thread;

protected slots:

    void onStartClicked();
    void onStopClicked();
    void onThreadFinished();
    void onPacketReceived(const ReceiveTimestamp recv_ts);
};

#endif // RECVTIMESTAMPWIDGET_H

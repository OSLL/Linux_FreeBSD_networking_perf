#ifndef SENDTIMESTAMPWIDGET_H
#define SENDTIMESTAMPWIDGET_H

#include <QWidget>
#include <QtCharts>

#include "../../utils/default_args.h"
#include "../../types/enums/MeasureType.h"
#include "../../utils/sockets.h"
#include "../../datasources/BaseDataSource.h"
#include "../threads/TimestampsSenderThread.h"
#include "../../types/TimestampsChart.h"
#include "startstopwidget.h"

//TODO: На графике softwate только в начале. Почему?
namespace Ui {
class SendTimestampWidget;
}

using namespace std::placeholders;

class SendTimestampWidget : public QWidget
{
    Q_OBJECT

public:
    SendTimestampWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~SendTimestampWidget();

protected:
    void changeEvent(QEvent *e);
    void recreateChart(quint64 packets_count, bool is_us);

protected slots:
    void onStartClicked();
    void onStopClicked();
    void onThreadFinished();
    void onPacketSent(std::optional<SendTimestamp> timestamp);

private:

    Ui::SendTimestampWidget *ui;
    TimestampsSenderThread *sender_thread;
    BaseDataSource *data_source;
    TimestampsChart *chart;
    StartStopWidget *start_stop;


    TimeSeries<QLineSeries> *software_series;
    TimeSeries<QLineSeries> *hardware_series;
    TimeSeries<QLineSeries> *in_call_series;

    QChartView chart_view;
};

#endif // SENDTIMESTAMPWIDGET_H

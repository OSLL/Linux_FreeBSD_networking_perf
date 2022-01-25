#ifndef RECVTIMESTAMPWIDGET_H
#define RECVTIMESTAMPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QtCharts>

#include "../../datasources/BaseDataSource.h"
#include "../../types/TimeSeries.h"
#include "../../types/DynamicAxisChart.h"
#include "../threads/TimestampsReceiverThread.h"

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

private:
    Ui::RecvTimestampWidget *ui;
    BaseDataSource *data_source;

    QChartView chart_view;

    TimeSeries<QLineSeries> *software_series;
    TimeSeries<QLineSeries> *hardware_series;
    TimeSeries<QLineSeries> *in_call_series;
    TimeSeries<QLineSeries> *total_series;

    TimestampsReceiverThread *receiver_thread;

protected slots:

    void onStartClicked();
    void onPacketReceived(const InSystemTimeInfo &time_info);
};

#endif // RECVTIMESTAMPWIDGET_H

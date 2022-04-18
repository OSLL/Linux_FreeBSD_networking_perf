#ifndef LINUXPKTGENWIDGET_H
#define LINUXPKTGENWIDGET_H

#include <QWidget>
#include "../../datasources/BaseDataSource.h"
#include "../../types/BandwidthChart.h"

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

private:
    Ui::LinuxPktgenWidget *ui;
    BaseDataSource *data_source;
    QChartView chart_view;
    BandwidthSeries *bandwidth_series;
    BandwidthChart *chart;
};

#endif // LINUXPKTGENWIDGET_H

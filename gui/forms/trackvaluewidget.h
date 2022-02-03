#ifndef TRACKVALUEWIDGET_H
#define TRACKVALUEWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QtCharts>

#include "../../datasources/BaseDataSource.h"
#include "../../types/series/TimeSeries.h"

namespace Ui {
class TrackValueWidget;
}

class TrackValueWidget : public QWidget
{
    Q_OBJECT

public:
    TrackValueWidget(std::function<int()> _get_value_func, const QString &prop_name, QWidget *parent = nullptr);
    ~TrackValueWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::TrackValueWidget *ui;
    std::function<int()> get_value_func;
    int value;
    QTimer update_timer;

    TimeSeries<QLineSeries> *series;
    QChartView chart_view;

private slots:
    void onTimeout();
};

#endif // TRACKVALUEWIDGET_H

//
// Created by shenk on 26.01.2022.
//

#ifndef LFNP_TIMESTAMPSCHART_H
#define LFNP_TIMESTAMPSCHART_H

#include "DynamicAxisChart.h"
#include "series/TimeSeries.h"
#include "series/FuncSeries.h"

class TimestampsChart: public DynamicYAxisChart {

private:

    bool in_us;
    QValueAxis *x_axis;

public:

    TimestampsChart(quint64 packets_count, bool in_us): DynamicYAxisChart(), in_us(in_us) {

        x_axis = new QValueAxis();
        QChart::addAxis(x_axis, Qt::AlignBottom);
        if (packets_count) {
            x_axis->setRange(0, packets_count);
        } else {
            x_axis->setRange(0, 100);
        }

    }

    void addSeries(QXYSeries **series) {
        if (in_us) {
            std::function<qreal(quint64)> to_us = [](quint64 ns_val) {return ns_val/1000;};
            *series = new FuncSeries<TimeSeries<QLineSeries>, qreal, quint64>(to_us);
        } else {
            *series = new TimeSeries<QLineSeries>();
        }
        DynamicYAxisChart::addSeries(*series);
        (*series)->attachAxis(x_axis);
    }

};

#endif //LFNP_TIMESTAMPSCHART_H

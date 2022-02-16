//
// Created by shenk on 26.01.2022.
//

#ifndef LFNP_TIMESTAMPSCHART_H
#define LFNP_TIMESTAMPSCHART_H

#include "series/TimeSeries.h"
#include "series/FuncSeries.h"
#include "AdvancedChart.h"

typedef FuncSeries<TimeSeries<QLineSeries>, qreal, quint64> TimestampSeries;

class TimestampsChart: public AdvancedChart {

private:

    bool in_us;

    std::function<qreal(quint64)> to_us = [](quint64 ns_val) {return ns_val/1000;};
    std::function<qreal(quint64)> id = [](quint64 ns_val) {return ns_val;};

    void initTitle() {
        auto y_axis = getYAxis();
        if (in_us) {
            y_axis->setTitleText(tr("Time, us"));
        } else {
            y_axis->setTitleText(tr("Time, ns"));
        }
    }

public:

    TimestampsChart(quint64 packets_count, bool in_us): AdvancedChart(), in_us(in_us) {

        setPacketsCount(packets_count);
        initTitle();

    }

    void setPacketsCount(quint64 packets_count) {

        if (packets_count) {
            getXAxis()->setRange(0, packets_count);
        } else {
            getXAxis()->setRange(0, 100);
        }

    }

    void setUS(bool _in_us) {
        in_us = _in_us;
        for (auto *series: series()) {
            auto timestamp_series = dynamic_cast<TimestampSeries*>(series);
            if (timestamp_series) {
                timestamp_series->setFunc(in_us ? to_us : id);
            }
        }
        initTitle();
    }

    void addSeries(TimestampSeries **series) {
        *series = new TimestampSeries(in_us ? to_us : id);
        AdvancedChart::addSeries(*series);
    }

    void clear() {
        AdvancedChart::clear();
        for (auto *series: series()) {
            auto timestamp_series = dynamic_cast<TimestampSeries*>(series);
            if (timestamp_series) {
                timestamp_series->resetCounter();

            }
        }
    }

};

#endif //LFNP_TIMESTAMPSCHART_H

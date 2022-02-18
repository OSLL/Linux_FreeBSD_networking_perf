//
// Created by shenk on 18.02.2022.
//

#ifndef LFNP_BANDWIDTHCHART_H
#define LFNP_BANDWIDTHCHART_H

#include "AdvancedChart.h"
#include "BandwidthResult.h"
#include "enums/UnitsPrefixes.h"
#include "enums/BandwidthUnits.h"
#include "series/TimeSeries.h"
#include "series/FuncSeries.h"

typedef FuncSeries<TimeSeries<QLineSeries>, qreal, BandwidthResult> BandwidthSeries;

class BandwidthChart: public AdvancedChart {

private:

    QMap<UnitsPrefixes, quint64> division_factor = {
            {NONE, 1},
            {KILO, 1000},
            {MEGA, 1000000},
            {GIGA, 1000000000L}
    };

    UnitsPrefixes current_prefix;
    BandwidthUnits current_unit;

    void initValueFunc() {
        auto div_factor = division_factor[current_prefix];
        auto get_unit_func = current_unit == BandwidthUnits::BYTES ? get_bytes : get_packets;
        get_value_func = [div_factor, get_unit_func](BandwidthResult res) {
            return get_unit_func(res)/div_factor;
        };
    }

    void initValue() {

        initValueFunc();

        for (auto *series: series()) {
            auto bandwidth_series = dynamic_cast<BandwidthSeries*>(series);
            if (bandwidth_series) {
                bandwidth_series->setFunc(get_value_func);
            }
        }

    }

    void initTitle() {
        auto y_axis = getYAxis();
        auto o_unit_name = bandwidth_units_enum.toString(current_unit);
        auto o_prefix_name = units_prefixes_enum.toString(current_prefix);
        if (o_unit_name && o_prefix_name) {
            auto unit_name = o_unit_name.value();
            auto prefix_name = o_prefix_name.value();

            QString axis_unit = prefix_name.front().toUpper() + unit_name;
            y_axis->setTitleText("Bandwidth, " + axis_unit);
        }
    }

    std::function<quint64(BandwidthResult)> get_bytes = [](BandwidthResult res) { return res.bytes_count; };
    std::function<quint64(BandwidthResult)> get_packets = [](BandwidthResult res) { return res.packets_count; };
    std::function<quint64(BandwidthResult)> get_value_func;

public:

    BandwidthChart(UnitsPrefixes _prefix, BandwidthUnits _unit): AdvancedChart() {
        current_prefix = _prefix;
        current_unit = _unit;

        initValueFunc();
        initTitle();
    }

    void setPrefix(UnitsPrefixes _prefix) {
        current_prefix = _prefix;
        initValue();
        initTitle();
    }

    void setUnit(BandwidthUnits _unit) {
        current_unit = _unit;
        initValue();
        initTitle();
    }

    void addSeries(BandwidthSeries **series) {
        *series = new BandwidthSeries(get_value_func);
        AdvancedChart::addSeries(*series);
    }

    void clear() {
        AdvancedChart::clear();
        for (auto *series: series()) {
            auto bandwidth_series = dynamic_cast<BandwidthSeries *>(series);
            if (bandwidth_series) {
                bandwidth_series->resetCounter();
            }
        }
    }

};

#endif //LFNP_BANDWIDTHCHART_H

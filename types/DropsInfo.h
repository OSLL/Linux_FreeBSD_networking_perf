//
// Created by shenk on 21.01.2022.
//

#ifndef LFNP_DROPSINFO_H
#define LFNP_DROPSINFO_H


struct DropsInfo {

    quint64 rx_drops;
    quint64 tx_drops;
    quint64 common_drops;
    bool is_common;

    DropsInfo(int rx_drops, int tx_drops): rx_drops(rx_drops), tx_drops(tx_drops), common_drops(0), is_common(false) {}
    explicit DropsInfo(int common_drops): rx_drops(0), tx_drops(0), common_drops(common_drops), is_common(true) {}
    explicit DropsInfo(): rx_drops(0), tx_drops(0), common_drops(0), is_common(false) {}

};


#endif //LFNP_DROPSINFO_H

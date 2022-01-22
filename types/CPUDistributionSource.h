//
// Created by shenk on 22.01.2022.
//

#ifndef LFNP_CPUDISTRIBUTIONSOURCE_H
#define LFNP_CPUDISTRIBUTIONSOURCE_H

#include <optional>
#include <QString>

enum CPUDistributionSource {
    SOFTIRQ_NET_RX,
    SOFTIRQ_NET_TX,
    SOFTNET_PROCESSED,
    NETISR_PROCESSED,
};

std::optional<CPUDistributionSource> getCPUDistributionSource(const QString &s_source);

#endif //LFNP_CPUDISTRIBUTIONSOURCE_H

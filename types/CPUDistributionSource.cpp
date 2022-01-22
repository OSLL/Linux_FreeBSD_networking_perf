//
// Created by shenk on 22.01.2022.
//

#include "CPUDistributionSource.h"

std::optional<CPUDistributionSource> getCPUDistributionSource(const QString &s_source) {
#ifdef __linux__
    if (s_source == "net-rx") {
        return SOFTIRQ_NET_RX;
    } else if (s_source == "net-tx") {
        return SOFTIRQ_NET_TX;
    } else if (s_source == "softnet-processed") {
        return SOFTNET_PROCESSED;
    }
#else
    if (s_source == "netisr") {
        return NETISR_PROCESSED;
    }
#endif
    return std::nullopt;
}
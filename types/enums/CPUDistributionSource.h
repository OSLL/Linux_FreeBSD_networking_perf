//
// Created by shenk on 22.01.2022.
//

#ifndef LFNP_CPUDISTRIBUTIONSOURCE_H
#define LFNP_CPUDISTRIBUTIONSOURCE_H

#include "BaseEnum.h"

enum CPUDistributionSource {
    SOFTIRQ_NET_RX,
    SOFTIRQ_NET_TX,
    SOFTNET_PROCESSED,
    NETISR_PROCESSED,
};

const Enum<CPUDistributionSource> cpu_distribution_source_enum({
#ifdef __linux__
        {SOFTIRQ_NET_RX, "net-rx"},
        {SOFTIRQ_NET_TX, "net-tx"},
        {SOFTNET_PROCESSED, "softnet-processed"},
#else
        {NETISR_PROCESSED, "netisr"}
#endif
});
#endif //LFNP_CPUDISTRIBUTIONSOURCE_H

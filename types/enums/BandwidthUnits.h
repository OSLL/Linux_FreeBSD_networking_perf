//
// Created by shenk on 18.02.2022.
//

#ifndef LFNP_BANDWIDTHUNITS_H
#define LFNP_BANDWIDTHUNITS_H

#include "BaseEnum.h"

enum BandwidthUnits {

    BITS,
    PKTS

};

const Enum<BandwidthUnits> bandwidth_units_enum({
    {BITS, "bits/s"},
    {PKTS, "pkts/s"},
});

#endif //LFNP_BANDWIDTHUNITS_H

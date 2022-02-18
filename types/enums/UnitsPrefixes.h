//
// Created by shenk on 18.02.2022.
//

#ifndef LFNP_UNITSPREFIXES_H
#define LFNP_UNITSPREFIXES_H

#include "BaseEnum.h"

enum UnitsPrefixes {

    GIGA,
    MEGA,
    KILO,
    NONE

};

const Enum<UnitsPrefixes> units_prefixes_enum({
    {GIGA, "Giga"},
    {MEGA, "Mega"},
    {KILO, "Kilo"},
    {NONE, "None"}
});

#endif //LFNP_UNITSPREFIXES_H

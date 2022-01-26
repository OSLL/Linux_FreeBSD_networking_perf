//
// Created by shenk on 24.01.2022.
//

#ifndef LFNP_MEASURETYPE_H
#define LFNP_MEASURETYPE_H

#include "BaseEnum.h"

enum MeasureType {

    SOFTWARE,
    HARDWARE,
    SCHEDULE,
    ACK

};

const Enum<MeasureType> measure_type_enum({
        {SOFTWARE, "software"},
#ifdef __linux__
        {HARDWARE, "hardware"},
        {SCHEDULE, "schedule"},
        {ACK, "ack"}
#endif
});


#endif //LFNP_MEASURETYPE_H

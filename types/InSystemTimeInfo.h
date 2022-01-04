//
// Created by shenk on 23.12.2021.
//

#ifndef LFNP_INSYSTEMTIMEINFO_H
#define LFNP_INSYSTEMTIMEINFO_H


class InSystemTimeInfo {

public:

    timespec software_time;
    timespec hardware_time;
    timespec total_time;

    InSystemTimeInfo():
            software_time({0, 0}),
            hardware_time({0, 0}),
            total_time({0, 0}){}
};

#endif //LFNP_INSYSTEMTIMEINFO_H

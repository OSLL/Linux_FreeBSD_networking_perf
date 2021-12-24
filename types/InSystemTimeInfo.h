//
// Created by shenk on 23.12.2021.
//

#ifndef LFNP_INSYSTEMTIMEINFO_H
#define LFNP_INSYSTEMTIMEINFO_H


class InSystemTimeRXInfo {

public:

    timespec rx_kernel_user_time;
    timespec rx_hardware_kernel_time;

    InSystemTimeRXInfo():
    rx_kernel_user_time({0, 0}), rx_hardware_kernel_time({0, 0}){}
};


#endif //LFNP_INSYSTEMTIMEINFO_H

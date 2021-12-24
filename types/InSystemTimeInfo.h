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

class InSystemTimeTXInfo {

public:

    timespec tx_user_scheduler_time;
    timespec tx_kernel_hardware_time;

    InSystemTimeTXInfo():
            tx_user_scheduler_time({0, 0}), tx_kernel_hardware_time({0, 0}){}

};

#endif //LFNP_INSYSTEMTIMEINFO_H

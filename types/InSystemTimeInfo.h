//
// Created by shenk on 23.12.2021.
//

#ifndef LFNP_INSYSTEMTIMEINFO_H
#define LFNP_INSYSTEMTIMEINFO_H


class InSystemTimeRXInfo {

public:

    timespec rx_software_time;
    timespec rx_hardware_time;

    InSystemTimeRXInfo():
            rx_software_time({0, 0}), rx_hardware_time({0, 0}){}
};

class InSystemTimeTXInfo {

public:

    timespec tx_software_time;
    timespec tx_hardware_time;

    InSystemTimeTXInfo():
            tx_software_time({0, 0}), tx_hardware_time({0, 0}){}

};

#endif //LFNP_INSYSTEMTIMEINFO_H

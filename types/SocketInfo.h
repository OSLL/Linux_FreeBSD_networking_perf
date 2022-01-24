//
// Created by shenk on 21.12.2021.
//

#ifndef LFNP_SOCKETINFO_H
#define LFNP_SOCKETINFO_H

#include <string>
#include <utility>

class SocketInfo {

public:

    const std::string local_address;
    const std::string foreign_address;

    const int local_port;
    const int foreign_port;

    const int rx_queue_size;
    const int tx_queue_size;

    const int ref;
    const int drops;

    SocketInfo(std::string local_address, std::string foreign_address,
               int local_port, int foreign_port,
               int rx_queue_size, int tx_queue_size,
               int ref, int drops)
            : local_address(std::move(local_address)), foreign_address(std::move(foreign_address)),
              local_port(local_port), foreign_port(foreign_port),
              rx_queue_size(rx_queue_size), tx_queue_size(tx_queue_size),
              ref(ref), drops(drops){}

};

#endif //LFNP_SOCKETINFO_H

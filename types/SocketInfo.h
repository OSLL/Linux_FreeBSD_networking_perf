//
// Created by shenk on 21.12.2021.
//

#ifndef LFNP_SOCKETINFO_H
#define LFNP_SOCKETINFO_H

#include <string>
#include <utility>

class SocketInfo {

public:

    const QString local_address;
    const QString foreign_address;

    const unsigned int local_port;
    const unsigned int foreign_port;

    const unsigned int rx_queue_size;
    const unsigned int tx_queue_size;

    const int ref;
    const int drops;

    SocketInfo(): local_address(), foreign_address(), local_port(0), foreign_port(0),
    rx_queue_size(0), tx_queue_size(0), ref(0), drops(0) {}

    SocketInfo(QString local_address, QString foreign_address,
               unsigned int local_port, unsigned int foreign_port,
               unsigned int rx_queue_size, unsigned int tx_queue_size,
               int ref, int drops)
            : local_address(std::move(local_address)), foreign_address(std::move(foreign_address)),
              local_port(local_port), foreign_port(foreign_port),
              rx_queue_size(rx_queue_size), tx_queue_size(tx_queue_size),
              ref(ref), drops(drops){}

};

#endif //LFNP_SOCKETINFO_H

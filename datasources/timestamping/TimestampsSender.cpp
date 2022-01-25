//
// Created by shenk on 25.01.2022.
//

#include "TimestampsSender.h"

TimestampsSender::TimestampsSender(Socket &sock, QFile &file, unsigned int data_size, bool zero_copy,
                                   SendProcessFunc &func):
                                   sock(sock), file(file), data_size(data_size), zero_copy(zero_copy),
                                   send_process_func(func) {

    if (!zero_copy) {
        data = file.read(data_size);
    }

    sock.sendData(&data_size);
}

void TimestampsSender::sendOne() {

    std::optional<TimeRange> o_timerange;
    if (zero_copy) {
        o_timerange = sock.sendFile(file.handle(), data_size);
    } else {
        o_timerange = sock.sendData(data, data_size);
    }
    if (!o_timerange) {
        std::cout << "Error in send data" << std::endl;
        return;
    }

    send_process_func(sock, time_info, *o_timerange);
    sock.sendData(&o_timerange->from);

    time_info.in_call_time.push_back(o_timerange->getRangeNS());
}

const InSystemTimeInfo &TimestampsSender::getInfo() {
    return time_info;
}



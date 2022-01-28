//
// Created by shenk on 25.01.2022.
//

#include "TimestampsSender.h"

TimestampsSender::TimestampsSender(Socket &_sock, std::unique_ptr<QFile> &_file, quint64 _data_size, bool _zero_copy,
                                   SendProcessFunc &_func):
                                   sock(_sock), file(std::move(_file)), data_size(_data_size), zero_copy(_zero_copy),
                                   send_process_func(_func) {

    if (!zero_copy) {
        data = file->read(data_size);
    }

    sock.sendData(&data_size);
}

std::optional<SendTimestamp> TimestampsSender::sendOne() {

    SendTimestamp timestamp;
    std::optional<TimeRange> o_timerange;
    if (zero_copy) {
        o_timerange = sock.sendFileTS(file->handle(), data_size);
    } else {
        o_timerange = sock.sendDataTS(data, data_size);
    }

    if (!o_timerange) {
        std::cout << "Error in send data" << std::endl;
        return std::nullopt;
    }

    send_process_func(sock, timestamp, *o_timerange);
    sock.sendData(&o_timerange->from);

    timestamp.before_send = o_timerange->from;
    timestamp.after_send = o_timerange->to;

    time_info.push_back(timestamp);
    return timestamp;
}

const QVector<SendTimestamp> &TimestampsSender::getInfo() {
    return time_info;
}



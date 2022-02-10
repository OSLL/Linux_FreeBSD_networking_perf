//
// Created by shenk on 25.01.2022.
//

#include "TimestampsSender.h"

TimestampsSender::TimestampsSender(Socket &_sock, const QString &protocol, std::unique_ptr<QFile> &_file, quint64 _data_size, bool _zero_copy,
                                   BaseDataSource *ds, MeasureType measure_type):
                                   control_sock(_sock), send_sock(protocol), file(std::move(_file)), data_size(_data_size), zero_copy(_zero_copy),
                                   data_source(ds) {

    if (!zero_copy) {
        data = file->read(data_size);
    }

    control_sock.sendData(&data_size);

    unsigned int send_port = 0;
    control_sock.receiveData(&send_port);

    send_sock.connectTo(control_sock.getAddr(), send_port);
    data_source->setSendSockOpt(send_sock, measure_type);
}

std::optional<SendTimestamp> TimestampsSender::sendOne() {

    bool is_next = true;
    control_sock.sendData(&is_next);

    SendTimestamp timestamp;
    std::optional<TimeRange> o_timerange;
    if (zero_copy) {
        o_timerange = send_sock.sendFileTS(file->handle(), data_size);
    } else {
        o_timerange = send_sock.sendDataTS(data, data_size);
    }

    if (!o_timerange) {
        std::cout << "Error in send data" << std::endl;
        return std::nullopt;
    }
    control_sock.sendData(&o_timerange->from);
    data_source->processSendTimestamp(send_sock, timestamp, *o_timerange);

    timestamp.before_send = o_timerange->from;
    timestamp.after_send = o_timerange->to;

    time_info.push_back(timestamp);
    return timestamp;
}

const QVector<SendTimestamp> &TimestampsSender::getInfo() {
    return time_info;
}



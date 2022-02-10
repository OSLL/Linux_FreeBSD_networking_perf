//
// Created by shenk on 23.12.2021.
//

#ifndef LFNP_INSYSTEMTIMEINFO_H
#define LFNP_INSYSTEMTIMEINFO_H

#include "TimeRange.h"

class ReceiveTimestamp {

public:

    timespec before_recv;
    timespec after_recv;
    std::optional<timespec> software_recv;
    std::optional<timespec> hardware_recv;
    timespec before_send;

    quint64 getInCallTime() const { return TimeRange(before_recv, after_recv).getRangeNS(); }
    quint64 getTotalTime() const { return TimeRange(before_send, after_recv).getRangeNS(); }

    std::optional<quint64> getSoftwareTime() const {
        if (software_recv) {
            return TimeRange(software_recv.value(), after_recv).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

    std::optional<quint64> getHardwareTime() const {
        if (hardware_recv) {
            return TimeRange(hardware_recv.value(), after_recv).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

};

class SendTimestamp {

public:

    timespec before_send;
    timespec after_send;
    std::optional<timespec> software_send;
    std::optional<timespec> hardware_send;

    quint64 getInCallTime() const { return TimeRange(before_send, after_send).getRangeNS(); }
    quint64 getTotalTime() const { return 0; }

    std::optional<quint64> getSoftwareTime() const {
        if (software_send) {
            return TimeRange(before_send, software_send.value()).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

    std::optional<quint64> getHardwareTime() const {
        if (hardware_send) {
            return TimeRange(before_send, hardware_send.value()).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

};

#endif //LFNP_INSYSTEMTIMEINFO_H

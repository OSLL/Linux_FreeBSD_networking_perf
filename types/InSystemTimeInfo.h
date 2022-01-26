//
// Created by shenk on 23.12.2021.
//

#ifndef LFNP_INSYSTEMTIMEINFO_H
#define LFNP_INSYSTEMTIMEINFO_H

#include "TimeRange.h"

class ITimestamp {

public:

    virtual quint64 getInCallTime() const = 0;
    virtual quint64 getTotalTime() const = 0;
    virtual std::optional<quint64> getSoftwareTime() const = 0;
    virtual std::optional<quint64> getHardwareTime() const = 0;
};

class ReceiveTimestamp: public ITimestamp{

public:

    timespec before_recv;
    timespec after_recv;
    std::optional<timespec> software_recv;
    std::optional<timespec> hardware_recv;
    timespec before_send;

    quint64 getInCallTime() const override { return TimeRange(before_recv, after_recv).getRangeNS(); }
    quint64 getTotalTime() const override { return TimeRange(before_send, after_recv).getRangeNS(); }

    std::optional<quint64> getSoftwareTime() const override {
        if (software_recv) {
            return TimeRange(software_recv.value(), after_recv).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

    std::optional<quint64> getHardwareTime() const override {
        if (hardware_recv) {
            return TimeRange(hardware_recv.value(), after_recv).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

};

class SendTimestamp: public ITimestamp{

public:

    timespec before_send;
    timespec after_send;
    std::optional<timespec> software_send;
    std::optional<timespec> hardware_send;

    quint64 getInCallTime() const override { return TimeRange(before_send, after_send).getRangeNS(); }
    quint64 getTotalTime() const override { return 0; }

    std::optional<quint64> getSoftwareTime() const override {
        if (software_send) {
            return TimeRange(software_send.value(), after_send).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

    std::optional<quint64> getHardwareTime() const {
        if (hardware_send) {
            return TimeRange(hardware_send.value(), after_send).getRangeNS();
        } else {
            return std::nullopt;
        }
    }

};

#endif //LFNP_INSYSTEMTIMEINFO_H

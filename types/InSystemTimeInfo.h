//
// Created by shenk on 23.12.2021.
//

#ifndef LFNP_INSYSTEMTIMEINFO_H
#define LFNP_INSYSTEMTIMEINFO_H

class InSystemTimeInfo {

public:

    QVector<quint64> software_time;
    QVector<quint64> hardware_time;
    QVector<quint64> in_call_time;
    QVector<quint64> total_time;

    quint64 getAverage(const QVector<quint64> &v) {

        double avg = 0;
        int size = v.size();

        for (auto e: v) {
            avg += (double)e/size;
        }

        return (quint64)avg;
    }

    quint64 getAverageSoftware() { return getAverage(software_time); }
    quint64 getAverageHardware() { return getAverage(hardware_time); }
    quint64 getAverageInCall() { return getAverage(in_call_time); }
    quint64 getAverageTotal() { return getAverage(total_time); }
};

#endif //LFNP_INSYSTEMTIMEINFO_H

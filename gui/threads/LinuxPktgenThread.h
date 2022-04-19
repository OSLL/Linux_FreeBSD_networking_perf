//
// Created by shenk on 18.04.2022.
//

#ifndef LFNP_LINUXPKTGENTHREAD_H
#define LFNP_LINUXPKTGENTHREAD_H

#include <QThread>
#include <utility>
#include "../../datasources/bandwidth/LinuxPktgen.h"
#include "../../types/BandwidthResult.h"

#define ITER_PACKETS_COUNT 1000

class LinuxPktgenThread: public QThread {

    Q_OBJECT

private:
    LinuxPktgen pktgen;
    quint64 packets_count;
    quint64 iter_packets_count;

protected:
    void run() override {

        for (quint64 sent_packets_count = 0;
        sent_packets_count < packets_count && !QThread::isInterruptionRequested();
        sent_packets_count += iter_packets_count) {
            pktgen.start();
            auto bandwidth_result = pktgen.getResult();
            if (bandwidth_result)
                emit onBandwidthResult(bandwidth_result.value());
        }
    }

public:

    LinuxPktgenThread(LinuxPktgen _pktgen, quint64 _packets_count, quint64 _iter_packets_count):
    pktgen(std::move(_pktgen)), packets_count(_packets_count), iter_packets_count(_iter_packets_count) {}

    ~LinuxPktgenThread() {
        pktgen.stop();
        pktgen.reset();
    }

    signals:

    void onBandwidthResult(BandwidthResult result);

};

#endif //LFNP_LINUXPKTGENTHREAD_H

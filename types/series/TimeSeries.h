//
// Created by shenk on 20.01.2022.
//

#ifndef LFNP_TIMESERIES_H
#define LFNP_TIMESERIES_H

#include <QtGlobal>
#include <QDebug>

#include "BaseSeries.h"

template <typename T>
class TimeSeries: public BaseSeries<T, quint64>{

public:
    TimeSeries();
    void append(quint64 y) override;
    quint64 getCounter();

private:

    quint64 counter;

};

template<typename T>
TimeSeries<T>::TimeSeries(): counter(0) {}

template<typename T>
void TimeSeries<T>::append(quint64 y) {
    T::append(counter, y);
    counter += 1;
}

template<typename T>
quint64 TimeSeries<T>::getCounter() {
    return counter;
}

#endif //LFNP_TIMESERIES_H

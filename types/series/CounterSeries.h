//
// Created by shenk on 20.01.2022.
//

#ifndef LFNP_COUNTERSERIES_H
#define LFNP_COUNTERSERIES_H

#include <QtGlobal>

#include "BaseSeries.h"

template <typename T>
class CounterSeries: public BaseSeries<T, qreal>{

public:
    CounterSeries();
    void append(qreal y);
    quint64 getCounter();
    void resetCounter();

private:

    quint64 counter;

};

template<typename T>
CounterSeries<T>::CounterSeries(): counter(0) {}

template<typename T>
void CounterSeries<T>::append(qreal y) {
    T::append(counter, y);
    counter += 1;
}

template<typename T>
quint64 CounterSeries<T>::getCounter() {
    return counter;
}

template<typename T>
void CounterSeries<T>::resetCounter() {
    counter = 0;
}

#endif //LFNP_COUNTERSERIES_H

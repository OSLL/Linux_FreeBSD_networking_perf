//
// Created by shenk on 20.01.2022.
//

#ifndef LFNP_TIMESERIES_H
#define LFNP_TIMESERIES_H

#include <QtGlobal>
#include <QDebug>

template <typename T>
class TimeSeries: public T{

public:
    TimeSeries();
    void append(qreal y);
    quint64 getCounter();

private:

    quint64 counter;

};

template<typename T>
TimeSeries<T>::TimeSeries(): counter(0) {}

template<typename T>
void TimeSeries<T>::append(qreal y) {
    T::append(counter, y);
    counter += 1;
}

template<typename T>
quint64 TimeSeries<T>::getCounter() {
    return counter;
}

#endif //LFNP_TIMESERIES_H

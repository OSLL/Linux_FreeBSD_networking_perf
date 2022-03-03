//
// Created by shenk on 20.01.2022.
//

#ifndef LFNP_TIMESERIES_H
#define LFNP_TIMESERIES_H

#include <QtGlobal>
#include <QDebug>

#include "BaseSeries.h"

template <typename T>
class TimeSeries: public BaseSeries<T, qreal>{

public:
    TimeSeries();
    void append(qreal y);
    quint64 getCounter();
    void resetCounter();
    void replace(QVector<qreal>);

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

template<typename T>
void TimeSeries<T>::resetCounter() {
    counter = 0;
}

template<typename T>
void TimeSeries<T>::replace(QVector<qreal> values) {

    QVector<QPointF> time_points;
    for (int i=0; i<values.size(); i++) {
        time_points.push_back(QPointF(i,values[i]));
    }
    qDebug() << time_points;
    T::replace(time_points);

}

#endif //LFNP_TIMESERIES_H

//
// Created by shenk on 19.03.2022.
//

#ifndef LFNP_TIMESERIES_H
#define LFNP_TIMESERIES_H

#include <QtGlobal>
#include <QDateTime>

#include "BaseSeries.h"

template <typename T>
class TimeSeries: public BaseSeries<T, qreal>{

public:

    void append(qreal y) {

        T::append(QDateTime::currentDateTime().toMSecsSinceEpoch(), y);

    }

};

#endif //LFNP_TIMESERIES_H

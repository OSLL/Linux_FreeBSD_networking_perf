//
// Created by shenk on 26.01.2022.
//

#ifndef LFNP_FUNCSERIES_H
#define LFNP_FUNCSERIES_H

#include <functional>
#include "BaseSeries.h"

template <class T, typename H, typename V>
class FuncSeries: public BaseSeries<T, V> {

private:
    std::function<H(V)> func;

public:

    explicit FuncSeries(std::function<H(V)> _func): func(_func) {}

    void append(V val) override {
        T::append(func(val));
    }

    void setFunc(const std::function<H(V)> &_func) {
        func = _func;
    }

};

#endif //LFNP_FUNCSERIES_H

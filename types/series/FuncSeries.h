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
    QVector<V> values;

public:

    explicit FuncSeries(std::function<H(V)> _func): func(_func) {}

    void append(V val) override {
        values.append(val);
        T::append(func(val));
    }

    void setFunc(const std::function<H(V)> &_func) {

        func = _func;

    }

    void replace(QVector<V> values) override {

        QVector<H> func_values;
        for (const auto& v: values) {
            func_values.push_back(func(v));
        }

        T::replace(func_values);
    }

};

#endif //LFNP_FUNCSERIES_H

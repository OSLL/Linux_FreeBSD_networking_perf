//
// Created by shenk on 26.01.2022.
//

#ifndef LFNP_BASESERIES_H
#define LFNP_BASESERIES_H

template <class T, typename H>
class BaseSeries: public T {

public:

    virtual void append(H)=0;

};

#endif //LFNP_BASESERIES_H

//
// Created by shenk on 24.01.2022.
//

#ifndef LFNP_DEFAULT_ARGS_H
#define LFNP_DEFAULT_ARGS_H

#include <QString>
#include <QMap>

const QMap<QString, QString> default_args = {
        {"protocol", "tcp"},
        {"port", "31415"},
        {"packets-count", "100"},
        {"address", "127.0.0.1"},
        {"measure-type", "software_recv"},
        {"delay", "0"},
        {"data", "/dev/urandom"},
        {"data-size", "0"}
};

#endif //LFNP_DEFAULT_ARGS_H

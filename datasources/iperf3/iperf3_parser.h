//
// Created by shenk on 05.01.2022.
//

#ifndef LFNP_IPERF3_PARSER_H
#define LFNP_IPERF3_PARSER_H

#include <iostream>
#include <optional>

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>

std::optional<QJsonArray> parseIperf3Output(const QString &filename);

#endif //LFNP_IPERF3_PARSER_H

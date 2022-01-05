//
// Created by shenk on 05.01.2022.
//

#include "iperf3_parser.h"



std::optional<QJsonArray> parseIperf3Output(const QString &filename) {

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    QJsonArray json_data;
    QByteArray current_data;
    auto error = QJsonParseError();

    while (!file.atEnd()) {

        auto line = file.readLine();
        current_data.append(line);

        if (line == "}\n") {
            auto current_json_data = QJsonDocument::fromJson(current_data, &error).object();
            json_data.append(current_json_data);
        }
    }

    return json_data;

}

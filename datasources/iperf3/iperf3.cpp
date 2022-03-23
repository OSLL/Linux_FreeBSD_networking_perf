//
// Created by shenk on 05.01.2022.
//

#include "iperf3.h"

std::optional<QVector<BandwidthResult>> Iperf3::parseIperf3Output(const QString &filename) {

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

    QVector<BandwidthResult> result;
    for (const auto& json_test: json_data) {
        auto json_test_o = json_test.toObject();
        for (const auto &json_interval: json_test_o["intervals"].toArray()) {
            auto json_interval_o = json_interval.toObject();
            auto json_sum_o = json_interval_o["sum"].toObject();
            result.push_back(BandwidthResult(0,
                                             json_sum_o["bytes"].toDouble(),
                                             json_sum_o["seconds"].toDouble()));
        }
    }

    return result;
}

std::function<void()> Iperf3::openFile(QTabWidget *tab_widget) {

    return [tab_widget]() {
        auto file_name = QFileDialog::getOpenFileName(nullptr,QFileDialog::tr("Open Iperf3 file"));

        if (!file_name.isEmpty()) {
            auto o_result = parseIperf3Output(file_name);
            if (o_result) {
                auto *widget = new BandwidthWidget(o_result.value(), {BandwidthUnits::BITS});
                int index = tab_widget->addTab(widget, file_name.split('/').last());
                tab_widget->setCurrentIndex(index);
            }
        }
    };
}

//
// Created by shenk on 04.03.2022.
//

#include "netmap.h"

QMap<QString, quint64> NetmapPktGen::unit_multiplier = {
    {"pps", 1},
    {"Kpps", 1000},
    {"Mpps", 1000000},
    {"Gpps", 1000000000},
    {"bps", 1},
    {"Kbps", 1000},
    {"Mbps", 1000000},
    {"Gbps", 1000000000}
};

std::optional<QVector<BandwidthResult>> NetmapPktGen::parseNetmapPktgenOutput(const QString &filename) {

    QRegularExpression re(R"((\d+\.\d+) main_thread \[\d+\] (\d+\.\d+) (\w+) \((\d+\.\d+) (\w+) (\d+\.\d+) (\w+) in (\d+) usec\))");
    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    QVector<BandwidthResult> result;

    while (!file.atEnd()) {
        auto file_line = file.readLine();
        auto file_line_match = re.match(file_line);

        if (file_line_match.hasMatch()) {
            const double pps = file_line_match.captured(2).toDouble();
            const QString pps_unit = file_line_match.captured(3);
            const double bps = file_line_match.captured(6).toDouble();
            const QString bps_unit = file_line_match.captured(7);

            result.push_back(BandwidthResult(
                    pps * unit_multiplier[pps_unit],
                    bps * unit_multiplier[bps_unit],
                    1
            ));
        }
    }

    return result;
}

std::function<void()> NetmapPktGen::openFile(QTabWidget *tab_widget) {
    return [tab_widget]() {
        auto file_name = QFileDialog::getOpenFileName(nullptr,QFileDialog::tr("Open Netmap Pktgen file"));

        if (!file_name.isEmpty()) {
            auto o_result = parseNetmapPktgenOutput(file_name);
            if (o_result) {
                auto *widget = new BandwidthWidget(o_result.value(),
                                                   {BandwidthUnits::BITS, BandwidthUnits::PKTS});
                tab_widget->addTab(widget, file_name.split('/').last());
            }
        }
    };
}

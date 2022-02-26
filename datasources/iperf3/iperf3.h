//
// Created by shenk on 05.01.2022.
//

#ifndef LFNP_IPERF3_H
#define LFNP_IPERF3_H

#include <iostream>
#include <optional>

#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtCore/QJsonParseError>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonObject>
#include <QFileDialog>
#include <QTabWidget>

#include "../../gui/forms/bandwidthwidget.h"
#include "../../types/BandwidthResult.h"


class Iperf3 {

public:

    static std::optional<QVector<BandwidthResult>> parseIperf3Output(const QString &filename);
    static std::function<void()> openFile(QTabWidget *tab_widget);

};

#endif //LFNP_IPERF3_H

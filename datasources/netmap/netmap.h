//
// Created by shenk on 03.03.2022.
//

#ifndef LFNP_NETMAP_H
#define LFNP_NETMAP_H

#include <optional>
#include <iostream>

#include <QFile>
#include <QFileDialog>
#include <QString>
#include <QTabWidget>

#include "../../types/BandwidthResult.h"
#include "../../gui/forms/bandwidthwidget.h"

class NetmapPktGen {
public:

    static QMap<QString, quint64> unit_multiplier;

    static std::optional<QVector<BandwidthResult>> parseNetmapPktgenOutput(const QString &filename);

    static std::function<void()> openFile(QTabWidget *tab_widget);

};

#endif //LFNP_NETMAP_H

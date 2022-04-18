//
// Created by shenk on 18.04.2022.
//

#ifndef LFNP_DROPSINFOMODEL_H
#define LFNP_DROPSINFOMODEL_H

#include <QAbstractListModel>
#include <QDebug>
#include <utility>
#include <optional>

#include "../../types/DropsInfo.h"

class DropsInfoModel: public QAbstractListModel {

private:

    QVector<QPair<QString, DropsInfo>> drops_info;
    const QVector<QString> header = {"Source", "RX Drops", "TX Drops"};

public:

    DropsInfoModel(QVector<QPair<QString, DropsInfo>> _drops_info): drops_info(std::move(_drops_info)) {}

    int rowCount(const QModelIndex &parent) const override {
        return drops_info.size();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {

        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            return header[section];
        }
        return {};
    }

    int columnCount(const QModelIndex &parent) const override {
        return 3;
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            auto drops_info_source = drops_info[index.row()].first;
            auto drops_info_elem = drops_info[index.row()].second;
            switch (index.column()) {
                case 0:
                    return drops_info_source;
                case 1:
                    return drops_info_elem.is_common ?
                           drops_info_elem.common_drops : drops_info_elem.rx_drops;
                case 2:
                    return drops_info_elem.tx_drops;
            }
        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignmentFlag::AlignCenter;
        } else if (role == Qt::BackgroundRole) {
            auto drops_info_elem = drops_info[index.row()].second;
            if ((drops_info_elem.is_common && drops_info_elem.common_drops)
            || (drops_info_elem.rx_drops || drops_info_elem.tx_drops)) {
                return QBrush(Qt::red);
            }
        }

        return {};
    }

    QSize span(const QModelIndex &index) const override {
        if (index.column() == 1) {
            auto drops_info_elem = drops_info[index.row()].second;
            if (drops_info_elem.is_common) {
                return {2, 1};
            }
        }
        return {0, 0};
    }

    void setData(QVector<QPair<QString, DropsInfo>> _drops_info) {
        drops_info = std::move(_drops_info);
    }

    QString sourceAt(int row) {
        return drops_info[row].first;
    }

    std::optional<DropsInfo> getDrops(QString source) {
        for (const auto drops_source: drops_info) {
            if (drops_source.first == source) {
                return drops_source.second;
            }
        }
        return std::nullopt;
    }

};

#endif //LFNP_DROPSINFOMODEL_H

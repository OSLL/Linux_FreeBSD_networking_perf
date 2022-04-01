//
// Created by shenk on 02.02.2022.
//

#ifndef LFNP_PROTOCOLSTATSMODEL_H
#define LFNP_PROTOCOLSTATSMODEL_H

#include <QAbstractListModel>
#include "../../datasources/BaseDataSource.h"

class ProtocolStatsModel: public QAbstractListModel {

private:

    ProtocolStats stats;
    QStringList names;
    const QVector<QString> header;

public:

    ProtocolStatsModel(): header({"Name", "Value"}) {}

    explicit ProtocolStatsModel(ProtocolStats _stats):
    stats(std::move(_stats)),
    names(stats.keys()),
    header({"Name", "Value"}) {}

    int rowCount(const QModelIndex &parent) const override {
        return stats.size();
    }

    int columnCount(const QModelIndex &parent) const override {
        return header.size();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {

        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            return header[section];
        }

        return {};
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return {Qt::ItemFlag::ItemIsEnabled, Qt::ItemFlag::ItemIsSelectable};
    }

    QVariant data(const QModelIndex &index, int role) const override {

        if (role == Qt::DisplayRole) {
            auto name = names[index.row()];

            switch (index.column()) {
                case 0:
                    return name;
                case 1:
                    return stats.value(name);
            }

        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignmentFlag::AlignLeft;
        }

        return {};

    }

    void setData(ProtocolStats _stats) {
        beginResetModel();
        stats = std::move(_stats);
        names = stats.keys();
        endResetModel();
    }

};

#endif //LFNP_PROTOCOLSTATSMODEL_H

//
// Created by shenk on 31.01.2022.
//

#ifndef LFNP_SOCKETSLISTMODEL_H
#define LFNP_SOCKETSLISTMODEL_H


#include <QAbstractListModel>
#include "../../types/SocketInfo.h"

class SocketsListModel: public QAbstractListModel {

private:

    QVector<SocketInfo> values;
    const QVector<QString> header = {"Local IP", "Local port", "Foreign IP", "Foreign port",
                                     "TX Queue", "RX Queue", "Refs", "Drops"};

public:

    SocketsListModel() = default;
    explicit SocketsListModel(const QVector<SocketInfo> &_values): values(_values) {}

    int rowCount(const QModelIndex &parent) const override {
        return values.size();
    }

    QVariant data(const QModelIndex &index, int role) const override {

        if (role == Qt::DisplayRole) {
            auto &socket_info = values[index.row()];

            switch (index.column()) {
                case 0:
                    return socket_info.local_address;
                case 1:
                    return socket_info.local_port;
                case 2:
                    return socket_info.foreign_address;
                case 3:
                    return socket_info.foreign_port;
                case 4:
                    return socket_info.tx_queue_size;
                case 5:
                    return socket_info.rx_queue_size;
#ifdef __linux__
                case 6:
                    return socket_info.ref;
                case 7:
                    return socket_info.drops;
#endif
            }
        } else if (role == Qt::TextAlignmentRole) {
            return Qt::AlignmentFlag::AlignLeft;
        } else if (role == Qt::BackgroundRole) {
            auto &socket_info = values[index.row()];
            if (socket_info.drops) {
                return QBrush(Qt::red);
            }
        }

        return {};
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const override {

        if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
            return header[section];
        }
        return {};
    }

    int columnCount(const QModelIndex &parent) const override {
#ifdef __linux__
        return 8;
#else
        return 6;
#endif
    }

    Qt::ItemFlags flags(const QModelIndex &index) const override {
        return {Qt::ItemFlag::ItemIsEnabled, Qt::ItemFlag::ItemIsSelectable};
    }

    void setData(QVector<SocketInfo> &_values) {
        values = _values;
    }

    SocketInfo dataByRow(int row) {
        return values[row];
    }
};


#endif //LFNP_SOCKETSLISTMODEL_H

//
// Created by shenk on 16.04.2022.
//

#ifndef LFNP_PROFILERPROTOCOLSMODEL_H
#define LFNP_PROFILERPROTOCOLSMODEL_H

#include <QAbstractItemModel>

class ProfilerProtocolsModel: public QAbstractListModel {

private:

    QStringList model_data;
    QVector<bool> checked_data;

public:
    ProfilerProtocolsModel(QStringList _data):
    QAbstractListModel(),
    model_data(_data),
    checked_data(model_data.size(), false) {}

    int rowCount(const QModelIndex &parent) const override {
        return model_data.size();
    }

    int columnCount(const QModelIndex &parent) const override {
        return 1;
    }

    QVariant data(const QModelIndex &index, int role) const override {
        if (role == Qt::DisplayRole) {
            return model_data[index.row()];
        } else if (role == Qt::CheckStateRole) {
            return checked_data[index.row()] ? Qt::Checked : Qt::Unchecked;
        }

        return {};
    }

    void check(int index) {
        if (index < checked_data.size()) {
            checked_data[index] = !checked_data[index];
        }
    }

    QString displayText() {
        QStringList display;
        for (int i=0; i<model_data.size(); i++) {
            if (checked_data[i]) {
                display.push_back(model_data[i]);
            }
        }

        // Ни малейшего понятия, почему тут обязан быть пробел, но если его убрать, то появляется странный баг
        // - в qcombobox происходит событие activated при простом открытии выбора, а не при нажатии на элемент
        return display.join(",") + " ";
    }

    QVector<QString> getChecked() {
        QVector<QString> checked;
        for (int i=0; i<model_data.size(); i++) {
            if (checked_data[i]) {
                checked.push_back(model_data[i]);
            }
        }
        return checked;
    }
};

#endif //LFNP_PROFILERPROTOCOLSMODEL_H

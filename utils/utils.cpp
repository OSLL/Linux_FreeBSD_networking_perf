//
// Created by shenk on 18.12.2021.
//

#include <QFile>
#include <iostream>
#include "utils.h"

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}

timespec timespecsub(timespec &tsp, timespec &usp) {
    timespec vsp {
            .tv_sec = tsp.tv_sec - usp.tv_sec,
            .tv_nsec = tsp.tv_nsec - usp.tv_nsec
    };

    if (vsp.tv_nsec < 0) {
        vsp.tv_sec--;
        vsp.tv_nsec += 1000000000L;
    }

    return vsp;
}

bool is_timespec_empty(timespec &tsp) {
    return (tsp.tv_sec == 0 && tsp.tv_nsec == 0);
}

int timespeccmp(timespec &ts1, timespec &ts2) {
    if (ts1.tv_sec == ts2.tv_sec) {
        return ts1.tv_nsec - ts2.tv_nsec;
    } else {
        return ts1.tv_sec - ts2.tv_sec;
    }

}

std::optional<quint64> get_int_from_file(const QString &filename) {

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    return file.readLine().toUInt();
}

#include <QDebug>
std::optional<std::unique_ptr<QFile>> get_file(const QString &filename, quint64 data_size) {
    auto file = std::make_unique<QFile>(filename);

    if (!file->open(QIODevice::ReadOnly)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    auto file_size = file->size();
    if (!file_size) {
        auto tmp_file = std::make_unique<QTemporaryFile>();

        if (!tmp_file->open()) {
            std::cout << "Can't open " << tmp_file->fileName().toStdString() << std::endl;
            return std::nullopt;
        }

        tmp_file->write(file->read(data_size));
        tmp_file->close();
        tmp_file->open();
        return tmp_file;
    }

    return file;
}

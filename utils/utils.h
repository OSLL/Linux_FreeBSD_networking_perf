//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_UTILS_H
#define LFNP_UTILS_H

#include <optional>
#include <memory>
#include <iostream>

#include <QString>
#include <QFile>
#include <QTemporaryFile>

// Сравнение времен. 1 - ts1 > ts2, -1 - ts1 < ts2, 0 - ts1 == ts2
int timespeccmp(timespec &ts1, timespec &ts2);

// Проверяет, что время - не пустое. В качетсве пустого принимается время, сикунды и наносекунды которого равны нулю.
// Используется для того, что бы понять, поддерживается ли системой определенный timestamp.
bool is_timespec_empty(timespec &tsp);

std::optional<quint64> get_int_from_file(const QString &filename);

template<typename T>
quint64 get_average(QVector<T> &v, std::function<quint64(T&)> func) {
    quint64 avg = 0;

    for (auto e:v) {
        avg += func(e);
    }

    return avg/v.size();
}

template<typename T>
quint64 get_average_o(QVector<T> &v, std::function<std::optional<quint64>(T&)> func) {
    quint64 avg = 0;
    quint64 size = 0;

    for (auto e:v) {
        auto o_val = func(e);
        if (o_val) {
            size++;
        }
    }

    if (size) {
        for (auto e:v) {
            auto o_val = func(e);
            if (o_val) {
                avg += *o_val/size;
            }
        }
        return avg;
    } else return 0;
}

// Получение файла с длиной. Нужно, что бы жопустить использование таких файлов, как /dev/urandom вместе с sendfile
// Получает файл, открывает и проверяет длину. Если длина существует (не равна 0), то возвращает файл. Иначе, создает
// временный файл с заданной длиной.
std::optional<std::unique_ptr<QFile>> get_file(const QString &filename, quint64 data_size);

#endif //LFNP_UTILS_H

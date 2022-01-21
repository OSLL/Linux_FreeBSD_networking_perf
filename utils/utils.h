//
// Created by shenk on 17.12.2021.
//

#ifndef LFNP_UTILS_H
#define LFNP_UTILS_H

#include <vector>
#include <string>
#include <sstream>
#include <optional>
#include <QString>

std::vector<std::string> split(const std::string &s, char delim);

// Здесь tsp - время события, которое произошло позже, чем usp. То есть usp < tsp.
timespec timespecsub(timespec &tsp, timespec &usp);

// Сравнение времен. 1 - ts1 > ts2, -1 - ts1 < ts2, 0 - ts1 == ts2
int timespeccmp(timespec &ts1, timespec &ts2);

// Проверяет, что время - не пустое. В качетсве пустого принимается время, сикунды и наносекунды которого равны нулю.
// Используется для того, что бы понять, поддерживается ли системой определенный timestamp.
bool is_timespec_empty(timespec &tsp);

// Функция, которая вычисляет разницу междк from и to, после чего прибавляет в res разницу, поделенное на total_count
// Используется в функциях вида getInSystemTime для вычисления средней разницы для всех пакетов
void timespec_avg_add(timespec &res, timespec &from, timespec &to, unsigned int total_count);

bool is_timespec_equal(timespec &tsc1, timespec &tsc2);

std::optional<quint64> get_int_from_file(const QString &filename);

#endif //LFNP_UTILS_H

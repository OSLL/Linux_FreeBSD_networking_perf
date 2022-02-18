//
// Created by shenk on 24.01.2022.
//

#ifndef LFNP_BASEENUM_H
#define LFNP_BASEENUM_H

#include <QStringList>
#include <QMap>
#include <optional>

template<typename T>
class Enum {

public:

    Enum(QVector<QPair<T, QString>> enum_map) {

        for (const auto &[enum_val, enum_str]: enum_map) {
            enum_string[enum_val] = enum_str;
            string_enum[enum_str] = enum_val;
        }

    }

    std::optional<T> fromString(const QString& val) const {
        if (string_enum.contains(val)) {
            return string_enum.value(val);
        } else {
            return std::nullopt;
        }
    }

    std::optional<QString> toString(const T val)const {
        if (enum_string.contains(val)) {
            return enum_string.value(val);
        } else {
            return std::nullopt;
        }
    }

    QStringList allStrings() const {
        return string_enum.keys();
    }

protected:

    QMap<T, QString> enum_string;
    QMap<QString, T> string_enum;

};

#endif //LFNP_BASEENUM_H

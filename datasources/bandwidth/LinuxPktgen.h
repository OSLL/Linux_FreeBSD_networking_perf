//
// Created by shenk on 31.01.2022.
//

#ifndef LFNP_LINUXPKTGEN_H
#define LFNP_LINUXPKTGEN_H

#include <QString>
#include <QDir>
#include <iostream>
#include <QDebug>

//TODO: CPU affinity
class LinuxPktgen {

private:

    QDir pktgen_dir;

    const QString interface;
    const quint64 cpu_count;

    QMap<QString, QString> parseDeviceFile(const QString &device) {

        QRegularExpression re(R"((\w+):\s((\S+)?))");
        QMap<QString, QString> result;

        QFile device_file(pktgen_dir.filePath(device));
        if (!device_file.open(QIODevice::ReadOnly | QIODevice::WriteOnly)) {
            return result;
        }
        QString device_file_info = device_file.readAll();

        int offset = 0;
        auto match = re.match(device_file_info, offset);
        while (match.hasMatch()) {
            result[match.captured(1)] = match.captured(2);
            offset = match.capturedEnd(1);
            match = re.match(device_file_info, offset);
        }

        QRegularExpression count_re(R"(count (\d+))");
        auto count_match = count_re.match(device_file_info);
        result["count"] = count_match.captured(1);

        return result;
    }

    std::optional<BandwidthResult> parseDeviceResult(const QString &device) {

        QRegularExpression result_re(R"((\d+)pps.+\((\d+)bps\))");

        QFile device_file(pktgen_dir.filePath(device));
        if (!device_file.open(QIODevice::ReadOnly | QIODevice::WriteOnly)) {
            return std::nullopt;
        }
        QString last_line = device_file.readLine();
        while (!device_file.atEnd()) { last_line = device_file.readLine(); }

        auto result_match = result_re.match(last_line);
        if (!result_match.hasMatch()) { return std::nullopt; }
        auto pps = result_match.captured(1).toULongLong();
        auto bps = result_match.captured(2).toULongLong();

        return BandwidthResult(pps, bps, 1);
    }

    bool pgctrl_cmd(const QString &command) {
        QFile pgctrl(pktgen_dir.filePath("pgctrl"));
        if (!pgctrl.open(QIODevice::WriteOnly | QIODevice::Text)) {
            return false;
        }

        // Без +1 не работает. По умолчанию write передает в качестве count длину строки,
        // но код linux (/net/core/pktgen.c) рассчитывает на то, что строка передастся вместе с терминальным символом
        // (\0 на конце), поэтому нужен +1
        pgctrl.write(command.toLocal8Bit(), command.size()+1);
        return true;
    }

    void pg_set(QFile &file, const QString cmd) {
        file.reset();
        file.write(cmd.toLocal8Bit(), cmd.size()+1);
    }

public:

    LinuxPktgen(const QString &protocol, const QString &_interface, const QString &dst_ip, const unsigned int dst_port,
                const QString &dst_mac, const quint64 _cpu_count, const quint64 data_size, const quint64 packets_count):
                pktgen_dir("/proc/net/pktgen"), cpu_count(_cpu_count), interface(_interface) {

        if (!pktgen_dir.exists()) {
            std::cout << "Directory /proc/net/pktgen don't exist. Try to use \"modprobe pktgen\"" << std::endl;
            return;
        }

        reset();

        int cpu_i = 0;
        for (const auto &entry: pktgen_dir.entryList({"kpktgend_*"}, QDir::Filter::NoFilter, QDir::SortFlag::Name)) {

            QFile file(pktgen_dir.filePath(entry));
            if (!file.open(QIODevice::ReadWrite)) {
                std::cout << "Can't open " << file.fileName().toStdString() << ". Are you have permission for this?" << std::endl;
                return;
            }

            QString add_device_cmd = "add_device " + interface + "@" + QString::number(cpu_i);
            file.write(add_device_cmd.toLocal8Bit());

            cpu_i++;
            if (cpu_i == cpu_count) break;
        }

        for (const auto &device_entry: pktgen_dir.entryList({interface+"@*"})) {
            QFile file(pktgen_dir.filePath(device_entry));
            if (!file.open(QIODevice::WriteOnly)) {
                std::cout << "Can't open " << file.fileName().toStdString() << std::endl;
                return;
            }

            pg_set(file, "count " + QString::number(packets_count));
            pg_set(file, "pkt_size " + QString::number(data_size));
            pg_set(file, "dst_mac " + dst_mac);
            pg_set(file, "udp_dst_min " + QString::number(dst_port));
            pg_set(file, "udp_dst_max " + QString::number(dst_port));

            if (protocol.endsWith('6')) {
                pg_set(file, "dst6 " + dst_ip);
            } else {
                pg_set(file, "dst " + dst_ip);
            }
        }
    }

    void start() {
        pgctrl_cmd("start");
    }

    void stop() {
        pgctrl_cmd("stop");
    }

    void reset() {
        pgctrl_cmd("reset");
    }

    std::optional<BandwidthResult> getResult() {

        BandwidthResult res;

        for (quint64 cpu_i=0; cpu_i < cpu_count; cpu_i++) {
            auto cur_res = parseDeviceResult(interface + "@" + QString::number(cpu_i));
            if (!cur_res) return std::nullopt;

            res.bytes_count += cur_res->bytes_count;
            res.packets_count += cur_res->packets_count;
        }

        res.duration = 1;
        return res;
    }

};


#endif //LFNP_LINUXPKTGEN_H

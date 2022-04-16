//
// Created by shenk on 15.03.2022.
//

#ifndef LFNP_PROFILER_PARSERS_H
#define LFNP_PROFILER_PARSERS_H

#include <QFile>
#include <optional>
#include <QTextStream>
#include <iostream>
#include "../../types/FuncProfiler.h"

#define TOKENS_COUNT 6

typedef QMap<int, QVector<FuncProfilerTreeNode*>> CPUProfilerData;
typedef QMap<quint64, CPUProfilerData> ProfilerData;

class ProfilerParser {

private:
    QMap<quint64, QMap<int, QVector<FuncProfilerToken>>> pid_tokens;
    quint64 last_token_timestamp;
    void init(QTextStream &in, quint64 from_timestamp);
    void _getProfilerTree(const QVector<FuncProfilerToken> &tokens, int &token_index, FuncProfilerTreeNode *parent,
                                                          const FuncProfilerToken &enter_token);
public:

    ProfilerParser(): last_token_timestamp(0) {}
    explicit ProfilerParser(const QString &filename, quint64 from_timestamp);
    explicit ProfilerParser(QTextStream &in, quint64 from_timestamp);

    QVector<FuncProfilerTreeNode*> getProfilerTrees(int cpu, quint64 pid);
    quint64 getLastTokenTimestamp() const {
        return last_token_timestamp;
    }

    QList<int> getAvailableCPUs(quint64 pid) const {
        return pid_tokens[pid].keys();
    }

    QList<quint64> getAvailablePids() const {
        return pid_tokens.keys();
    }

    std::optional<QVector<FuncProfilerToken>> getTokens(int cpu, quint64 pid) {
        if (pid_tokens.contains(pid)) {
            if (pid_tokens[pid].contains(cpu)) {
                return pid_tokens[pid][cpu];
            }
        }
        return std::nullopt;
    }

};

#endif //LFNP_PROFILER_PARSERS_H

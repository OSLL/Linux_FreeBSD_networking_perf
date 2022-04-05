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

#define TOKENS_COUNT 4

class ProfilerParser {

private:
    QMap<int, QVector<FuncProfilerToken>> cpu_tokens;
    quint64 last_token_timestamp;
    void init(QTextStream &in, quint64 from_timestamp);
    void _getProfilerTree(const QVector<FuncProfilerToken> &tokens, int &token_index, FuncProfilerTreeNode *parent,
                                                          const FuncProfilerToken &enter_token);
public:

    ProfilerParser(): last_token_timestamp(0) {}
    explicit ProfilerParser(const QString &filename, quint64 from_timestamp);
    explicit ProfilerParser(QTextStream &in, quint64 from_timestamp);

    QVector<FuncProfilerTreeNode*> getProfilerTrees(int cpu);
    quint64 getLastTokenTimestamp() const {
        return last_token_timestamp;
    }

    QList<int> getAvailableCPUs() const {
        return cpu_tokens.keys();
    }

    std::optional<QVector<FuncProfilerToken>> getTokens(int cpu) {
        if (cpu_tokens.contains(cpu)) {
            return cpu_tokens.value(cpu);
        }
        return std::nullopt;
    }

};

#endif //LFNP_PROFILER_PARSERS_H

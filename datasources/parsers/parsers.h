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
    void init(QTextStream &in);
    void _getProfilerTree(const QVector<FuncProfilerToken> &tokens, int &token_index, FuncProfilerTreeNode *parent,
                                                          const FuncProfilerToken &enter_token);
public:

    explicit ProfilerParser(const QString &filename);
    explicit ProfilerParser(QTextStream &in);

    std::optional<FuncProfilerTreeNode*> getProfilerTree();

};

#endif //LFNP_PROFILER_PARSERS_H

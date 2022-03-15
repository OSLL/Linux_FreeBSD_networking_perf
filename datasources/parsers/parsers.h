//
// Created by shenk on 15.03.2022.
//

#ifndef LFNP_PARSERS_H
#define LFNP_PARSERS_H

#include <QFile>
#include <optional>
#include <QTextStream>
#include <iostream>
#include "../../types/FuncProfiler.h"

void parseProfilerData(QTextStream &in, FuncProfilerTreeNode *parent, quint64 enter_time);
std::optional<FuncProfilerTreeNode*> parseProfilerData(QTextStream &in);
std::optional<FuncProfilerTreeNode*> parseProfilerData(const QString &filename);

#endif //LFNP_PARSERS_H

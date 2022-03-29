//
// Created by shenk on 08.03.2022.
//

#ifndef LFNP_FUNCPROFILER_H
#define LFNP_FUNCPROFILER_H

#include <QString>
#include <QVector>
#include "TimeRange.h"

class FuncProfilerToken {

public:
    enum TokenType {
        ENTER,
        RETURN
    };

    QString func_name;
    int cpu_index;
    quint64 timestamp;
    TokenType type;

    FuncProfilerToken(const QStringList &token_list) {
        type = token_list[0] == "enter" ? ENTER : RETURN;
        func_name = token_list[1];
        timestamp = token_list[2].toULongLong();
        cpu_index = token_list[3].toInt();
    }
};

class FuncProfilerTreeNode {

private:

    QString func_name;
    TimeRangeNS time_range;
    int cpu_index;

    FuncProfilerTreeNode *parent;
    QVector<FuncProfilerTreeNode*> children;

public:

    FuncProfilerTreeNode(QString _func_name, int _cpu_index, FuncProfilerTreeNode *_parent):
    func_name(_func_name), cpu_index(_cpu_index), parent(_parent) {}

    void setRange(TimeRangeNS time_range) {
        this->time_range = time_range;
    }

    TimeRangeNS getRange() const {
        return time_range;
    }

    quint64 getDuration() const {
        return time_range.getRangeNS();
    }

    FuncProfilerTreeNode *getParent() {
        return parent;
    }

    const FuncProfilerTreeNode *getParentConst() const {
        return parent;
    }

    QString getFuncName() const {
        return func_name;
    }

    int getCPUIndex() const {
        return cpu_index;
    }

    QVector<FuncProfilerTreeNode*> getChildren() const {
        return children;
    }

    std::optional<FuncProfilerTreeNode*> getChild(QString func_name) {
        for (auto child: children) {
            if (child->func_name == func_name) {
                return child;
            }
        }
        return std::nullopt;
    }

    void addChildren(FuncProfilerTreeNode *child) {
        children.append(child);
    }

    bool isRoot() const {
        return parent == nullptr;
    }

};

#endif //LFNP_FUNCPROFILER_H

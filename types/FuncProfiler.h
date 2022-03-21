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
    QVector<TimeRangeNS> time_ranges;
    int cpu_index;

    FuncProfilerTreeNode *parent;
    QVector<FuncProfilerTreeNode*> children;

public:

    FuncProfilerTreeNode(QString _func_name, int _cpu_index, FuncProfilerTreeNode *_parent):
    func_name(_func_name), cpu_index(_cpu_index), parent(_parent) {}

    void addRange(TimeRangeNS time_range) {
        time_ranges.append(time_range);
    }

    quint64 getDuration() {

        if (!time_ranges.empty()) {

            quint64 avg_time = 0;
            for (const auto &time_range: time_ranges) {
//            std::cout << "DURATION " << time_range.getRangeNS() << std::endl;
                avg_time += time_range.getRangeNS();
            }
            return avg_time/time_ranges.size();

        }
        return 0;
    }

    FuncProfilerTreeNode *getParent() {
        return parent;
    }

    QString getFuncName() const {
        return func_name;
    }

    int getCPUIndex() const {
        return cpu_index;
    }

    QVector<FuncProfilerTreeNode*> getChildren() {
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

    void addChildrenAsTime(FuncProfilerTreeNode *child) {

        auto o_current_child = getChild(child->func_name);
        if (o_current_child) {
            auto current_child = o_current_child.value();
            for (auto time_range: child->time_ranges) {
                current_child->addRange(time_range);
            }

            for (auto next_child: child->getChildren()) {
                current_child->addChildrenAsTime(next_child);
            }
        } else {
            addChildren(child);
        }

    }

};

#endif //LFNP_FUNCPROFILER_H

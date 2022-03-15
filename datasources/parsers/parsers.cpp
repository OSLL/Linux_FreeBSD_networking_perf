//
// Created by shenk on 15.03.2022.
//

#include "parsers.h"

void parseProfilerData(QTextStream &in, FuncProfilerTreeNode *parent, quint64 enter_time) {

    bool is_return = false;
    while (!(is_return || in.atEnd()) ) {

        QString line = in.readLine();
        QStringList profiler_tokens = line.split(' ');

        if (profiler_tokens[0] == "enter") {

            auto child = new FuncProfilerTreeNode(
                    profiler_tokens[1], profiler_tokens[3].toInt(), parent);
            parseProfilerData(in, child, profiler_tokens[2].toULongLong());
            parent->addChildrenAsTime(child);

        } else if (profiler_tokens[0] == "return") {
            is_return = true;

            if (profiler_tokens[1] == parent->getFuncName()) {
                quint32 cpu_index = profiler_tokens[3].toUInt();
                parent->addRange(TimeRangeNS(enter_time, profiler_tokens[2].toULongLong()));

                if (cpu_index != parent->getCPUIndex()) {
                    std::cout << "Stack warning: different enter and return CPU" << std::endl;
                }
            } else {
                std::cout << "Stack error: different enter and return names" << std::endl;
            }

        }

    }
}

std::optional<FuncProfilerTreeNode*> parseProfilerData(QTextStream &in) {

    auto *root = new FuncProfilerTreeNode("root", 0, NULL);

    do {

        QString line = in.readLine();
        QStringList profiler_tokens = line.split(' ');

        if (profiler_tokens[0] == "enter") {

            auto child = new FuncProfilerTreeNode(
                    profiler_tokens[1], profiler_tokens[3].toInt(), root);
            parseProfilerData(in, child, profiler_tokens[2].toULongLong());
            root->addChildrenAsTime(child);

        } else if (profiler_tokens[0] == "return") {
            std::cout << "Warning: return at top level" << std::endl;
        }

    } while (!in.atEnd());

    return root;

}

std::optional<FuncProfilerTreeNode*> parseProfilerData(const QString &filename) {

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
        return std::nullopt;
    }

    QTextStream in(file.read(131000));
    return parseProfilerData(in);

}
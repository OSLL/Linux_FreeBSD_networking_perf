//
// Created by shenk on 15.03.2022.
//

#include "parsers.h"

ProfilerParser::ProfilerParser(const QString &filename, quint64 from_timestamp) {

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(file.read(131000));
        init(in, from_timestamp);
    } else {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
    }

}

ProfilerParser::ProfilerParser(QTextStream &in, quint64 from_timestamp) {
    init(in, from_timestamp);
}

void ProfilerParser::init(QTextStream &in, quint64 from_timestamp) {

    last_token_timestamp = from_timestamp;

    do {

        QString line = in.readLine();
        QStringList token_list = line.split(' ', Qt::SkipEmptyParts);
        if (token_list.size() == TOKENS_COUNT) {
            auto token = FuncProfilerToken(token_list);
            if (token.timestamp > from_timestamp) {
                if (token.timestamp > last_token_timestamp) {
                    last_token_timestamp = token.timestamp;
                }
                cpu_tokens[token.cpu_index][token.pid].push_back(token);
            }
        }

    } while (!in.atEnd());
}

void ProfilerParser::_getProfilerTree(const QVector<FuncProfilerToken> &tokens, int &token_index,
                                 FuncProfilerTreeNode *parent, const FuncProfilerToken &enter_token) {
    bool is_return = false;
    while (!is_return && token_index < tokens.size()) {
        const auto &token = tokens[token_index];
        token_index++;

        if (token.type == FuncProfilerToken::TokenType::ENTER) {

            auto child = new FuncProfilerTreeNode(
                    token.func_name, token.cpu_index, token.pid, parent);
            this->_getProfilerTree(tokens, token_index, child, token);
            if (child->getDuration() == 0) {
                std::cout << "Warning: function with zero duration - " << child->getFuncName().toStdString() << std::endl;
            }
            parent->addChildren(child);

        } else if (token.type == FuncProfilerToken::TokenType::RETURN) {
            is_return = true;

            if (token.func_name == parent->getFuncName()) {
                parent->setRange(TimeRangeNS(enter_token.timestamp, token.timestamp));

                if (token.cpu_index != parent->getCPUIndex()) {
                    std::cout << "Warning: different enter and return CPU" << std::endl;
                }
            } else {
                std::cout << "Warning: different enter and return names - " <<
                enter_token.func_name.toStdString() << " " << enter_token.cpu_index << " " << enter_token.pid << " " << enter_token.timestamp << " " <<
                token.func_name.toStdString() << " " << token.cpu_index << " " << token.pid << " " << token.timestamp << " " << std::endl;
            }
        }
    }
}

QVector<FuncProfilerTreeNode *> ProfilerParser::getProfilerTrees(int cpu, quint64 pid) {

    QVector<FuncProfilerTreeNode*> root_nodes;
    auto &tokens = cpu_tokens[cpu][pid];
    std::sort(tokens.begin(), tokens.end(), [](const FuncProfilerToken &tokenA, const FuncProfilerToken &tokenB) {
        return tokenA.timestamp < tokenB.timestamp;
    });

    int token_index = 0;
    while (token_index < tokens.size()) {
        const auto &token = tokens[token_index];
        token_index++;
        if (token.type == FuncProfilerToken::TokenType::ENTER) {

            auto root = new FuncProfilerTreeNode(
                    token.func_name, token.cpu_index, token.pid, nullptr);
            this->_getProfilerTree(tokens, token_index, root, token);
            if (root->getDuration()) {
                root_nodes.push_back(root);
            }

        } else if (token.type == FuncProfilerToken::TokenType::RETURN) {
            std::cout << "Warning: return at top level" << std::endl;
        }
    }

    return root_nodes;
}
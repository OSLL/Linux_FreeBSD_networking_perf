//
// Created by shenk on 15.03.2022.
//

#include "parsers.h"

ProfilerParser::ProfilerParser(const QString &filename) {

    QFile file(filename);

    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(file.read(131000));
        init(in);
    } else {
        std::cout << "Can't open " << filename.toStdString() << std::endl;
    }

}

ProfilerParser::ProfilerParser(QTextStream &in) {
    init(in);
}

void ProfilerParser::init(QTextStream &in) {

    do {

        QString line = in.readLine();
        QStringList token_list = line.split(' ', Qt::SkipEmptyParts);
        if (token_list.size() == TOKENS_COUNT) {
            auto token = FuncProfilerToken(token_list);
            cpu_tokens[token.cpu_index].push_back(token);
        }

    } while (!in.atEnd());

}

std::optional<FuncProfilerTreeNode *> ProfilerParser::getProfilerTree() {
    auto *root = new FuncProfilerTreeNode("root", 0, nullptr);

    for (const auto &tokens: cpu_tokens) {
        int token_index = 0;
        while (token_index < tokens.size()) {
            const auto &token = tokens[token_index];
            token_index++;
            if (token.type == FuncProfilerToken::TokenType::ENTER) {

                auto child = new FuncProfilerTreeNode(
                        token.func_name, token.cpu_index, root);
                this->_getProfilerTree(tokens, token_index, child, token);
                root->addChildrenAsTime(child);

            } else if (token.type == FuncProfilerToken::TokenType::RETURN) {
                std::cout << "Warning: return at top level" << std::endl;
            }
        }
    }

    return root;
}

void ProfilerParser::_getProfilerTree(const QVector<FuncProfilerToken> &tokens, int &token_index,
                                 FuncProfilerTreeNode *parent, const FuncProfilerToken &enter_token) {
    bool is_return = false;
    while (!is_return && token_index < tokens.size()) {
        const auto &token = tokens[token_index];
        token_index++;

        if (token.type == FuncProfilerToken::TokenType::ENTER) {

            auto child = new FuncProfilerTreeNode(
                    token.func_name, token.cpu_index, parent);
            this->_getProfilerTree(tokens, token_index, child, token);
            parent->addChildrenAsTime(child);

        } else if (token.type == FuncProfilerToken::TokenType::RETURN) {
            is_return = true;

            if (token.func_name == parent->getFuncName()) {
                parent->addRange(TimeRangeNS(enter_token.timestamp, token.timestamp));

                if (token.cpu_index != parent->getCPUIndex()) {
                    std::cout << "Stack warning: different enter and return CPU" << std::endl;
                }
            } else {
                std::cout << "Stack error: different enter and return names" << std::endl;
            }
        }
    }
}

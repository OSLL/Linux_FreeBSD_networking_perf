//
// Created by shenk on 21.03.2022.
//

#ifndef LFNP_FLAMEGRAPH_H
#define LFNP_FLAMEGRAPH_H

#include <QPainter>
#include <QOpenGLWidget>
#include "QDebug"
#include <QPaintEvent>
#include "../datasources/parsers/parsers.h"

#define RECT_HEIGHT 30

class FlameGraph: public QOpenGLWidget {

//    Q_OBJECT

public:

    FlameGraph(ProfilerParser parser): QOpenGLWidget(), parser(parser) {}

private:

    ProfilerParser parser;
    int paintToken(QPainter &painter,
                    const FuncProfilerToken &root_token,
                    const FuncProfilerToken &token,
                    const QVector<FuncProfilerToken> &tokens,
                    int &index, int &x_offset, int level) const {

        while (index < tokens.size()) {
            const auto &current_token = tokens[index];
            if (current_token.type == FuncProfilerToken::ENTER) {
                index++;
                paintToken(painter, root_token, current_token, tokens, index, x_offset, level+1);
            } else if (current_token.type == FuncProfilerToken::RETURN) {
                int width = (current_token.timestamp - token.timestamp) / 500;
                QRect func_rect(
                        x_offset + (token.timestamp - root_token.timestamp) / 500,
                        level * RECT_HEIGHT,
                        width,
                        RECT_HEIGHT
                );
                painter.drawRect(func_rect);
                auto text_width = QFontMetrics(painter.font()).horizontalAdvance(token.func_name);
                if (text_width < width) {
                    painter.drawText(func_rect, Qt::AlignCenter, token.func_name);
                }

                index++;
                return width;
            }
        }
        return -1;
    }

protected:
    void paintEvent(QPaintEvent *e) override {

        auto tokens = parser.getTokens(3).value();

        QPainter painter(this);
        painter.setPen(Qt::blue);

        int index = 0;
        int x_offset = 0;
        while (index < tokens.size()) {
            const auto &token = tokens[index];
            index++;
            if (token.type == FuncProfilerToken::ENTER && token.func_name == "ip_rcv") {
                int width = paintToken(painter, token, token, tokens, index, x_offset, 0);
                if (width > 0) {
                    x_offset += width + 10;
                }
            }
        }
    }


};


#endif //LFNP_FLAMEGRAPH_H

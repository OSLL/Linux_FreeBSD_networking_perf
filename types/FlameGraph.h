//
// Created by shenk on 21.03.2022.
//

#ifndef LFNP_FLAMEGRAPH_H
#define LFNP_FLAMEGRAPH_H

#include <QPainter>
#include <QOpenGLWidget>
#include "QDebug"
#include <QPaintEvent>
#include <QtMath>
#include <QToolTip>

#include "../datasources/parsers/parsers.h"

#define RECT_HEIGHT 30
#define NS_IN_PIXEL 300

class FlameGraph: public QOpenGLWidget {

//    Q_OBJECT

public:

    FlameGraph(ProfilerParser parser): QOpenGLWidget(), parser(parser), offset(0 ,0), scale(1) {
        this->setMouseTracking(true);
    }

private:

    QPointF last_mouse_pos;
    QPointF offset;
    qreal scale;
    QPoint hover;

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
                int duration = current_token.timestamp - token.timestamp;
                int width = duration / NS_IN_PIXEL * scale;
                int height = RECT_HEIGHT * scale;
                QRect func_rect(
                        x_offset + (token.timestamp - root_token.timestamp) / NS_IN_PIXEL * scale,
                        level * height,
                        width,
                        height
                );

                const QPoint offsetHover = (hover - offset).toPoint();
                if (func_rect.contains(offsetHover)) {
                    painter.fillRect(func_rect, Qt::gray);
                    QToolTip::showText(this->mapToGlobal(hover), QString("%1 (%2 ns)")
                    .arg(current_token.func_name, QString::number(duration)));
                }
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

        painter.eraseRect(e->rect());
        painter.fillRect(e->rect(), Qt::white);
        painter.translate(offset);
        painter.setPen(Qt::black);

        int index = 0;
        int x_offset = 0;
        while (index < tokens.size()) {
            const auto &token = tokens[index];
            index++;
            if (token.type == FuncProfilerToken::ENTER && token.func_name == "ip_rcv") {
                int width = paintToken(painter, token, token, tokens, index, x_offset, 0);
                if (width > 0) {
                    x_offset += width + 10*scale;
                }
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
        qDebug() << "Press";
        if (event->buttons() & Qt::MouseButton::LeftButton) {

            last_mouse_pos = event->pos();
            event->accept();

        }
    }

    void mouseMoveEvent(QMouseEvent *event) override {
        if (event->buttons() & Qt::MouseButton::LeftButton) {

            QPointF delta = event->pos() - last_mouse_pos;
            offset += delta;

            last_mouse_pos = event->pos();
            event->accept();

        }

        hover = event->pos();
        update();

        QOpenGLWidget::mouseMoveEvent(event);
    }

    void wheelEvent(QWheelEvent *event) override {
        qreal wheel_factor = qPow(1.1, (double)event->angleDelta().y()/50);
        scale *= wheel_factor;
        offset *= wheel_factor;

        update();
    }


};


#endif //LFNP_FLAMEGRAPH_H

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
        nodes = parser.getProfilerTrees(3);
    }

private:

    QVector<FuncProfilerTreeNode*> nodes;

    QPointF last_mouse_pos;
    QPointF offset;
    qreal scale;
    QPoint hover;

    ProfilerParser parser;
    int paintNode(QPainter &painter,
                    const FuncProfilerTreeNode *root_node,
                    const FuncProfilerTreeNode *node,
                    int &x_offset, int level) const {

        int duration = node->getDuration();
        int width = duration / NS_IN_PIXEL * scale;
        int height = RECT_HEIGHT * scale;

        QRect func_rect(
                x_offset + (node->getRange().from - root_node->getRange().from) / NS_IN_PIXEL * scale,
                level * height,
                width,
                height
        );

        const QPoint offsetHover = (hover - offset).toPoint();
        if (func_rect.contains(offsetHover)) {
            painter.fillRect(func_rect, Qt::gray);
            QString text;
            if (node->isRoot()) {
                text = QString("%1 (%2 ns)")
                        .arg(node->getFuncName(), QString::number(duration));
            } else {
                double of_parent = ((double)duration/node->getParentConst()->getDuration())*100;
                text = QString("%1 (%2 ns, %3% of parent)")
                        .arg(node->getFuncName(), QString::number(duration),
                             QString::number(of_parent));
            }
            QToolTip::showText(this->mapToGlobal(hover), text);
        }
        painter.drawRect(func_rect);

        auto text_width = QFontMetrics(painter.font()).horizontalAdvance(node->getFuncName());
        if (text_width < width) {
            painter.drawText(func_rect, Qt::AlignCenter, node->getFuncName());
        }

        for (const auto *child: node->getChildren()) {
            paintNode(painter, root_node, child, x_offset, level+1);
        }

        return width;
    }

protected:
    void paintEvent(QPaintEvent *e) override {

        QPainter painter(this);

        painter.eraseRect(e->rect());
        painter.fillRect(e->rect(), Qt::white);
        painter.translate(offset);
        painter.setPen(Qt::black);

        int x_offset = 0;
        for (const auto *node: nodes) {
            if (node->getFuncName() == "ip_rcv") {
                int width = paintNode(painter, node, node, x_offset, 0);
                if (width > 0) {
                    x_offset += width + 10*scale;
                }
            }
        }
    }

    void mousePressEvent(QMouseEvent *event) override {
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

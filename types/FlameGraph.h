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
#include <utility>

#include "../datasources/parsers/parsers.h"

#define RECT_HEIGHT 30
#define NS_IN_PIXEL 300


class FlameGraph: public QOpenGLWidget {

    Q_OBJECT

public:

    FlameGraph(): QOpenGLWidget(), offset(0, 0), scale(1), current_node(nullptr) {
        this->setMouseTracking(true);
    }

    FlameGraph(ProfilerParser parser, int cpu, quint64 pid): FlameGraph() {
        nodes = parser.getProfilerTrees(cpu, pid);
    }

    explicit FlameGraph(const QVector<FuncProfilerTreeNode *>& _nodes): FlameGraph() {
        nodes = _nodes;
    }

    void setNodes(const QVector<FuncProfilerTreeNode *>& _nodes) {
        offset = QPointF(0, 0);
        scale = 1;
        nodes = _nodes;
        update();
    }

private:

    QVector<FuncProfilerTreeNode*> nodes;
    const FuncProfilerTreeNode *current_node;

    QPointF last_mouse_pos;
    QPointF offset;
    qreal scale;
    QPoint hover;

    int paintNode(QPainter &painter,
                    const FuncProfilerTreeNode *root_node,
                    const FuncProfilerTreeNode *node,
                    const FuncProfilerTreeNode **pointed_node,
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
            *pointed_node = node;
            QToolTip::showText(this->mapToGlobal(hover), text);
        }
        painter.drawRect(func_rect);

        auto text_width = QFontMetrics(painter.font()).horizontalAdvance(node->getFuncName());
        if (text_width < width) {
            painter.drawText(func_rect, Qt::AlignCenter, node->getFuncName());
        }

        for (const auto *child: node->getChildren()) {
            if (child->getDuration()) {
                paintNode(painter, root_node, child, pointed_node, x_offset, level + 1);
            }
        }

        return width;
    }

protected:
    void paintEvent(QPaintEvent *e) override {

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.eraseRect(e->rect());

        painter.fillRect(e->rect(), Qt::white);

        painter.translate(offset);
        painter.setPen(Qt::black);

        current_node = nullptr;
        int x_offset = 0;
        for (const auto *node: nodes) {
            if (node->getDuration()) {
                int width = paintNode(painter, node, node, &current_node, x_offset, 0);
                if (width > 0) {
                    x_offset += width + 10*scale;
                }
            }
        }

        if (!current_node) {
            QToolTip::hideText();
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

    void mouseDoubleClickEvent(QMouseEvent *event) override {

        if (current_node) {
            emit nodeClick(event, current_node);
        }

        QWidget::mouseDoubleClickEvent(event);
    }

signals:

    void nodeClick(QMouseEvent*, const FuncProfilerTreeNode*);

};


#endif //LFNP_FLAMEGRAPH_H

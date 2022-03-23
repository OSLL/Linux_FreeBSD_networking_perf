//
// Created by shenk on 21.03.2022.
//

#ifndef LFNP_FLAMEGRAPH_H
#define LFNP_FLAMEGRAPH_H

#include <QPainter>
#include <QOpenGLWidget>
#include "QDebug"
#include <QPaintEvent>

#define RECT_HEIGHT 30

class FlameGraph: public QOpenGLWidget {

//    Q_OBJECT

public:

    FlameGraph(FuncProfilerTreeNode *root): QOpenGLWidget(), root(root) {
        root->addRange(TimeRangeNS(0, 240177));
    }

private:

    FuncProfilerTreeNode *root;
    void paintNode(QPainter &painter, FuncProfilerTreeNode *parent, QRect parent_rect) const {
        int x_offset = 0;
        for (const auto &child: parent->getChildren()) {
            qDebug() << parent->getDuration() << child->getDuration() << child->getFuncName() << x_offset;
            int width = parent_rect.width()*((qreal)child->getDuration() / parent->getDuration());
            qDebug() << width;
            QRect child_rect(
                    parent_rect.left() + x_offset,
                    parent_rect.top() - RECT_HEIGHT,
                    width,
                    RECT_HEIGHT
            );
            painter.drawRect(child_rect);
            auto text_width = QFontMetrics(painter.font()).horizontalAdvance(child->getFuncName());
            if (text_width < width) {
                painter.drawText(child_rect, Qt::AlignCenter, child->getFuncName());
            }
            x_offset += width;
            paintNode(painter, child, child_rect);
        }
    }

protected:
    void paintEvent(QPaintEvent *e) override {

        qDebug() << "paintEvent" << e;

        QPainter painter(this);
        painter.setPen(Qt::blue);

        paintNode(painter, root, QRect(0, e->rect().height(), e->rect().width(), RECT_HEIGHT));
    }


};


#endif //LFNP_FLAMEGRAPH_H

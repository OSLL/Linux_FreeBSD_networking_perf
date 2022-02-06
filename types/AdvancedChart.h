//
// Created by shenk on 03.02.2022.
//

#ifndef LFNP_ADVANCEDCHART_H
#define LFNP_ADVANCEDCHART_H

#include <QtCharts>
#include <QGraphicsItem>

//TODO: линии сверху и снизу
//TODO: поддержка datetimeseries
//TODO: выбор отображаемых series в ПКМ
//TODO: Функция отчистки
class AdvancedChart: public QChart {

private:

    qreal y_max, y_min, x_max;
    QAbstractAxis *x_axis;
    QAbstractAxis *y_axis;
    QPointF last_mouse_pos;

    bool auto_range, auto_scroll;

protected:

    void onPointAdded(const QPointF &point) {

        if (auto_range) {

            auto point_y = point.y();
            y_min = qMin(y_min, point_y);
            y_max = qMax(y_max, point_y);
            y_axis->setRange(y_min, y_max);
            x_axis->setRange(0, 100);
        }

        if (auto_scroll) {

            auto value_axis = dynamic_cast<QValueAxis*>(x_axis);
            if (value_axis) {
                if (point.x() > value_axis->max()) {
                    auto delta = point.x() - value_axis->max();
                    value_axis->setRange(value_axis->min() + delta, value_axis->max() + delta);
                }
            }

        }
    }

    void wheelEvent(QGraphicsSceneWheelEvent *event) override {
        auto_range = false;
        this->zoom(qPow(1.1, event->delta()/10));
    }

    void mousePressEvent(QGraphicsSceneMouseEvent *event) override {

        if (event->buttons() & Qt::MouseButton::LeftButton) {

            last_mouse_pos = event->pos();
            event->accept();

        }
        // Здесь не нужно вызывать mousePressEvent, при этом не вызывается mouseMoveEvent
        // QChart::mousePressEvent(event);
    }

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override {

        if (event->buttons() & Qt::MouseButton::LeftButton) {

            auto_scroll = false;
            auto_range = false;

            QPointF delta = event->pos() - last_mouse_pos;
            scroll(-delta.x(), delta.y());

            last_mouse_pos = event->pos();
            event->accept();

        }
        QChart::mouseMoveEvent(event);
    }


public:

    explicit AdvancedChart(qreal min=0, qreal max=1, QAbstractAxis *x=new QValueAxis, QAbstractAxis *y=new QValueAxis ):
            QChart(), y_min(min), y_max(max), y_axis(y), x_axis(x), auto_range(true), auto_scroll(true) {
        addAxis(y_axis, Qt::AlignmentFlag::AlignLeft);
        addAxis(x_axis, Qt::AlignmentFlag::AlignBottom);
    }

    void addSeries(QXYSeries *series) {
        QChart::addSeries(series);

        QObject::connect(series, &QXYSeries::pointAdded, this, [this, series](int index)
        {onPointAdded(series->at(index));});

        series->attachAxis(y_axis);
        series->attachAxis(x_axis);
    }

};

#endif //LFNP_ADVANCEDCHART_H

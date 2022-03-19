//
// Created by shenk on 03.02.2022.
//

#ifndef LFNP_ADVANCEDCHART_H
#define LFNP_ADVANCEDCHART_H

#include <QtCharts>
#include <QGraphicsItem>

//TODO: линии сверху и снизу
//TODO: выбор отображаемых series в ПКМ
//TODO: отображение среднего
class AdvancedChart: public QChart {

private:

    qreal y_max, y_min, x_max;
    const qreal start_max, start_min;
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
        }

        if (auto_scroll) {
            qreal current_max = 0;
            qreal current_min = 0;
            switch (x_axis->type()) {

                case QtCharts::QAbstractAxis::AxisTypeDateTime: {
                    auto date_axis = dynamic_cast<QDateTimeAxis*>(x_axis);
                    if (date_axis) {
                         current_max = date_axis->max().toMSecsSinceEpoch();
                         current_min = date_axis->min().toMSecsSinceEpoch();;
                    }
                    break;
                }

                default: {
                    auto value_axis = dynamic_cast<QValueAxis*>(x_axis);
                    if (value_axis) {
                        current_max = value_axis->max();
                        current_min = value_axis->min();
                    }
                    break;
                }
            }


            if (point.x() > current_max) {
                auto delta = point.x() - current_max;
                switch (x_axis->type()) {
                    case QtCharts::QAbstractAxis::AxisTypeDateTime: {
                        x_axis->setRange(QDateTime::fromMSecsSinceEpoch(current_min + delta),
                                         QDateTime::fromMSecsSinceEpoch(current_max + delta));
                        break;
                    }

                    default: {
                        x_axis->setRange(current_min + delta, current_max + delta);
                        break;
                    }

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

    AdvancedChart(qreal min=0, qreal max=1, QAbstractAxis *x=new QValueAxis, QAbstractAxis *y=new QValueAxis ):
            QChart(), y_min(min), y_max(max), start_min(min), start_max(max), y_axis(y), x_axis(x),
            auto_range(true), auto_scroll(true) {
        addAxis(y_axis, Qt::AlignmentFlag::AlignLeft);
        addAxis(x_axis, Qt::AlignmentFlag::AlignBottom);

        switch (x->type()) {
            case QtCharts::QAbstractAxis::AxisTypeDateTime: {
                QDateTime momentInTime = QDateTime::currentDateTime();
                x_axis->setRange(momentInTime, momentInTime.addSecs(100));
                break;
            }
            default: {
                x_axis->setRange(0, 100);
                break;
            }
        }
    }

    void addSeries(QXYSeries *series) {
        QChart::addSeries(series);

        QObject::connect(series, &QXYSeries::pointAdded, this, [this, series](int index)
        {onPointAdded(series->at(index));});

        series->attachAxis(y_axis);
        series->attachAxis(x_axis);
    }

    void clear() {

        for (auto series: QChart::series()) {
            if (series->type() == QtCharts::QAbstractSeries::SeriesTypeLine) {
                auto line_series = dynamic_cast<QLineSeries*>(series);
                line_series->clear();
            }
            y_min = start_min;
            y_max = start_max;
            y_axis->setRange(y_min, y_max);
        }

        auto_range = true;
        auto_scroll = true;
    }

    QAbstractAxis *getXAxis() const {
        return x_axis;
    }

    QAbstractAxis *getYAxis() const {
        return y_axis;
    }

};

#endif //LFNP_ADVANCEDCHART_H

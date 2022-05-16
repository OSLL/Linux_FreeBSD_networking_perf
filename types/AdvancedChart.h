//
// Created by shenk on 03.02.2022.
//

#ifndef LFNP_ADVANCEDCHART_H
#define LFNP_ADVANCEDCHART_H

#include <QtCharts>
#include <QGraphicsItem>

class AverageSeriesData {

protected:
    qreal data_sum;
    int length;

    QLineSeries *average_series;

public:

    AverageSeriesData(QLineSeries *_average_series): data_sum(0), length(0), average_series(_average_series) {}
    explicit AverageSeriesData(const QList<QPointF>& points_list, QLineSeries *_average_series):
    AverageSeriesData(_average_series) {
        length = points_list.size();
        for (const auto &point: points_list) {
            data_sum += point.y();
        }
    }

    void addPoint(QPointF point) {
        data_sum += point.y();
        length++;
    }

    qreal getAverage() const { return length ? data_sum/length : 0; }

    void updateSeries(qreal minX, qreal maxX) {
        auto next_average = getAverage();
        QPointF begin_point(minX, next_average), end_point(maxX, next_average);

        average_series->clear();
        average_series->append(begin_point);
        average_series->append(end_point);
    }

    QLineSeries *series() { return  average_series; }
    void clear() {
        average_series->clear();
        data_sum = 0;
        length = 0;
    }

};

//TODO: линии сверху и снизу
//TODO: выбор отображаемых series в ПКМ
class AdvancedChart: public QChart {

private:

    qreal y_max, y_min, x_max;
    const qreal start_max, start_min;
    QAbstractAxis *x_axis;
    QAbstractAxis *y_axis;
    QPointF last_mouse_pos;

    bool auto_range, auto_scroll;
    std::map<QXYSeries*, std::unique_ptr<AverageSeriesData>> average_series;

protected:

    void onPointAdded(QXYSeries *series, int index) {

        QPointF point = series->at(index);

        if (auto_range) {

            auto point_y = point.y();
            y_min = qMin(y_min, point_y);
            y_max = qMax(y_max, point_y);
            y_axis->setRange(y_min, y_max);
        }

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

        if (auto_scroll) {

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

        if (average_series.find(series) != average_series.end()) {
            auto &average_data = average_series[series];
            average_data->addPoint(point);
            average_data->updateSeries(current_min, current_max);
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

protected slots:
    void onRangeChanged(qreal min, qreal max) {
        for (const auto &average_series_it: average_series) {
            average_series_it.second->updateSeries(min, max);
        }
    };

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
                auto datetime_x_axis = dynamic_cast<QDateTimeAxis*>(x_axis);
                QObject::connect(datetime_x_axis, &QDateTimeAxis::rangeChanged, this, [this](QDateTime min, QDateTime max) {
                    this->onRangeChanged(min.toMSecsSinceEpoch(), max.toMSecsSinceEpoch());
                });
                break;
            }
            default: {
                x_axis->setRange(0, 100);
                auto value_x_axis = dynamic_cast<QValueAxis*>(x_axis);
                if (value_x_axis)
                    QObject::connect(value_x_axis, &QValueAxis::rangeChanged, this, &AdvancedChart::onRangeChanged);
                break;
            }
        }
    }

    void addSeries(QXYSeries *series, bool need_average=false) {
        QChart::addSeries(series);

        QObject::connect(series, &QXYSeries::pointAdded, this, [this, series](int index)
        {onPointAdded(series, index);});

        series->attachAxis(y_axis);
        series->attachAxis(x_axis);

        if (need_average) {
            auto *average_data_series = new QLineSeries;
            average_data_series->setName("Average");

            QColor series_color = series->pen().color();
            QPen average_pen(series_color);
            average_pen.setStyle(Qt::DashLine);
            average_data_series->setPen(average_pen);

            QChart::addSeries(average_data_series);
            average_data_series->attachAxis(y_axis);
            average_data_series->attachAxis(x_axis);

            average_series.insert(std::make_pair(series, std::make_unique<AverageSeriesData>(average_data_series)));
            auto average_series_marker = legend()->markers(average_data_series).first();
            average_series_marker->setBrush(QBrush(series_color, Qt::Dense2Pattern));
        }
    }

    void clear() {

        for (auto series: QChart::series()) {
            if (series->type() == QtCharts::QAbstractSeries::SeriesTypeLine) {
                auto line_series = dynamic_cast<QLineSeries*>(series);
                if (line_series)
                    line_series->clear();
            }
            y_min = start_min;
            y_max = start_max;
            y_axis->setRange(y_min, y_max);

            auto xy_series = dynamic_cast<QXYSeries*>(series);
            if (xy_series && average_series.find(xy_series) != average_series.end()) {
                average_series[xy_series]->clear();
            }
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

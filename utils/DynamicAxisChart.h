//
// Created by shenk on 20.01.2022.
//

#ifndef LFNP_DYNAMICAXISCHART_H
#define LFNP_DYNAMICAXISCHART_H

#include <QChart>
#include <QXYSeries>
#include <QValueAxis>

class DynamicYAxisChart: public QChart {

    Q_OBJECT

public:

    DynamicYAxisChart(): QChart(), y_max(std::nullopt) {
        this->addAxis(&y_axis, Qt::AlignLeft);
    }

    void addSeries(QXYSeries *series) {
        QChart::addSeries(series);
        QObject::connect(series, &QXYSeries::pointAdded, this, [this, series](int index)
        {onPointAdded(series->at(index));});
        series->attachAxis(&y_axis);
    }

private:
    std::optional<qreal> y_max;
    QValueAxis y_axis;

private slots:

    void onPointAdded(const QPointF &point) {
        if (!y_max || *y_max < point.y()) {
            y_max = point.y();
            y_axis.setRange(-10, *y_max+10);
        }
    }
};


#endif //LFNP_DYNAMICAXISCHART_H

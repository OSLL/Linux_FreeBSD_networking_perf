#include "trackvaluewidget.h"

#include <utility>
#include "ui_trackvaluewidget.h"
#include "../../types/DynamicAxisChart.h"

TrackValueWidget::TrackValueWidget(std::function<int()> _get_value_func, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackValueWidget),
    get_value_func(std::move(_get_value_func)),
    series(new TimeSeries<QLineSeries>)
{
    ui->setupUi(this);
    value = get_value_func();

    auto chart = new DynamicYAxisChart();
    chart->addSeries(series);
    auto x_axis = new QValueAxis();
    chart->addAxis(x_axis, Qt::AlignBottom);
    series->attachAxis(x_axis);
    series->append(value);

    chart_view.setChart(chart);
    ui->chartLayout->addWidget(&chart_view);

    update_timer.setInterval(1000);
    QObject::connect(&update_timer, &QTimer::timeout, this, &TrackValueWidget::onTimeout);
    update_timer.start();
}

TrackValueWidget::~TrackValueWidget()
{
    delete ui;
}

void TrackValueWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void TrackValueWidget::onTimeout() {

    value = get_value_func();
    series->append(value);

}

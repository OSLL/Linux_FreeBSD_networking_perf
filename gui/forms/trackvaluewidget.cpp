#include "trackvaluewidget.h"

#include <utility>
#include "ui_trackvaluewidget.h"

TrackValueWidget::TrackValueWidget(std::function<int()> _get_value_func, const QString &prop_name, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TrackValueWidget),
    get_value_func(std::move(_get_value_func)),
    series(new TimeSeries<QLineSeries>)
{
    ui->setupUi(this);
    value = get_value_func();

    auto chart = new AdvancedChart(value-1, value+1, new QDateTimeAxis);
    chart->addSeries(series);
    series->append(value);
    series->setName(prop_name);
    chart->getYAxis()->setTitleText(prop_name);

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
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

#include "displayvaluewidget.h"
#include "ui_displayvaluewidget.h"

DisplayValueWidget::DisplayValueWidget(
        QVector<std::pair<quint64, quint64>> data, QString series_name, bool need_average, QWidget *parent):
    QWidget(parent),
    ui(new Ui::DisplayValueWidget)
{
    ui->setupUi(this);

    auto chart_view = new QChartView();
    auto chart = new AdvancedChart(0, 1);

    auto series = new CounterSeries<QLineSeries>();
    series->setName(series_name);

//    chart->getXAxis()->setRange(QDateTime::fromMSecsSinceEpoch(data.first().second),
//                                QDateTime::fromMSecsSinceEpoch(data.last().second));
    chart->getXAxis()->setRange(0, data.size());
    chart->getXAxis()->setTitleText("Number");
    chart->getYAxis()->setTitleText("Time, ns");
    chart->addSeries(series, need_average);
    chart_view->setChart(chart);

    for (const auto &el: data) {
        series->append(el.first);
    }

    chart_view->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chart_view);
}

DisplayValueWidget::~DisplayValueWidget()
{
    delete ui;
}

void DisplayValueWidget::changeEvent(QEvent *e)
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

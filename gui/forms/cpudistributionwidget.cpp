#include "cpudistributionwidget.h"
#include "ui_cpudistributionwidget.h"

CPUDistributionWidget::CPUDistributionWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPUDistributionWidget),
    data_source(ds)
{
    ui->setupUi(this);

    timer = new QTimer(this);
    timer->setInterval(1000);
    QObject::connect(timer, &QTimer::timeout, this, &CPUDistributionWidget::onTimerTimeout);

    auto o_cpu_distribution = data_source->getCPUDistribution(SOFTIRQ_NET_RX);
    if (o_cpu_distribution) {
        auto cpu_distribution = o_cpu_distribution.value();

        auto *chart = new DynamicYAxisChart();
        x_axis = new QValueAxis();
        x_axis->setRange(0, 100);

        chart->addAxis(x_axis, Qt::AlignBottom);

        for (auto it=cpu_distribution.begin(); it != cpu_distribution.end(); it++) {
            series.push_back(new TimeSeries<QLineSeries>());
            series.last()->append(it.value());
            series.last()->setName("CPU"+QString::number(it.key()));
            chart->addSeries(series.last());
            series.last()->attachAxis(x_axis);
        }

        chart_view = new QChartView(chart);
        chart_view->setRenderHint(QPainter::Antialiasing);
        ui->verticalLayout->addWidget(chart_view);
        timer->start();

    }
}

CPUDistributionWidget::~CPUDistributionWidget()
{
    delete ui;
    delete timer;
    delete chart_view;
}

void CPUDistributionWidget::changeEvent(QEvent *e)
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

void CPUDistributionWidget::onTimerTimeout() {

    auto o_cpu_distribution = data_source->getCPUDistribution(SOFTIRQ_NET_RX);
    if (o_cpu_distribution) {
        auto cpu_distribution = o_cpu_distribution.value();

        auto cpu_it = cpu_distribution.begin();
        auto series_it = series.begin();
        for (; cpu_it != cpu_distribution.end(); cpu_it++, series_it++) {
            (*series_it)->append(cpu_it.value());
        }
    }

    quint64 counter = series.first()->getCounter();
    if (counter > x_axis->max()) {
        chart_view->chart()->scroll(series.first()->getCounter(), 0);
    }
}

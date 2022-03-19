#include "cpudistributionwidget.h"
#include "ui_cpudistributionwidget.h"

CPUDistributionWidget::CPUDistributionWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CPUDistributionWidget),
    chart(new AdvancedChart(0, 1, new QDateTimeAxis)),
    chart_view(new QChartView(chart)),
    data_source(ds),
    source(SOFTIRQ_NET_RX)
{
    ui->setupUi(this);
    ui->controlLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

    auto items = cpu_distribution_source_enum.allStrings();
    ui->sourceComboBox->addItems(items);
    if (items.size() < 2) {
        ui->sourceComboBox->setDisabled(true);
    }

    chart->getYAxis()->setTitleText(ui->sourceComboBox->currentText());
    auto o_cpu_distribution = data_source->getCPUDistribution(source);
    if (o_cpu_distribution) {
        auto cpu_distribution = o_cpu_distribution.value();

        for (auto it=cpu_distribution.begin(); it != cpu_distribution.end(); it++) {
            series.push_back(new TimeSeries<QLineSeries>());
            series.last()->append(it.value());
            series.last()->setName("CPU"+QString::number(it.key()));
            chart->addSeries(series.last());
        }
    }

    chart_view->setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(chart_view);

    timer = new QTimer(this);
    timer->setInterval(1000);
    timer->start();

    QObject::connect(timer, &QTimer::timeout, this, &CPUDistributionWidget::onTimerTimeout);
    QObject::connect(ui->sourceComboBox, &QComboBox::currentTextChanged, this, &CPUDistributionWidget::onSourceChanged);

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

    auto o_cpu_distribution = data_source->getCPUDistribution(source);
    if (o_cpu_distribution) {
        auto cpu_distribution = o_cpu_distribution.value();

        auto cpu_it = cpu_distribution.begin();
        auto series_it = series.begin();
        for (; cpu_it != cpu_distribution.end(); cpu_it++, series_it++) {
            (*series_it)->append(cpu_it.value());
        }
    }
}

void CPUDistributionWidget::onSourceChanged(const QString &_source) {

    source = cpu_distribution_source_enum.fromString(_source).value();

    chart->clear();
    chart->getYAxis()->setTitleText(_source);
    QDateTime momentInTime = QDateTime::currentDateTime();
    chart->getXAxis()->setRange(momentInTime, momentInTime.addSecs(100));

}

#include "bandwidthwidget.h"
#include "ui_bandwidthwidget.h"

BandwidthWidget::BandwidthWidget(const QVector<BandwidthResult>& _result, const QVector<BandwidthUnits>& available_units, QWidget *parent) :
    QWidget(parent),
    result(_result),
    ui(new Ui::BandwidthWidget)
{
    ui->setupUi(this);

    ui->settingsLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

    ui->prefixComboBox->addItems(units_prefixes_enum.allStrings());
    ui->prefixComboBox->setCurrentText(default_args["prefix"]);

    for (const auto unit: available_units) {
        ui->unitComboBox->addItem(bandwidth_units_enum.toString(unit).value());
    }
    ui->unitComboBox->setCurrentText(default_args["unit"]);
    if (available_units.size() == 1) ui->unitComboBox->setDisabled(true);

    chart = new BandwidthChart(GIGA, BITS);
    chart->addSeries(&bandwidth_series);
    bandwidth_series->setName("Bandwidth");

    recreateChart();

    chart->getXAxis()->setRange(0, result.size()-1);
    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(&chart_view);

    QObject::connect(ui->prefixComboBox, &QComboBox::currentTextChanged, this, &BandwidthWidget::prefixChanged);
}

BandwidthWidget::~BandwidthWidget()
{
    delete ui;
}

void BandwidthWidget::changeEvent(QEvent *e)
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

void BandwidthWidget::prefixChanged(const QString &text) {
    chart->clear();
    chart->setPrefix(units_prefixes_enum.fromString(text).value());
    recreateChart();
}

#include <QDebug>
void BandwidthWidget::recreateChart() {

    qDebug() << result.size();
    for (const auto &r: result) {
        bandwidth_series->append(r);
    }

}

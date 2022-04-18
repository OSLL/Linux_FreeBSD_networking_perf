#include "linuxpktgenwidget.h"
#include "ui_linuxpktgenwidget.h"

LinuxPktgenWidget::LinuxPktgenWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LinuxPktgenWidget),
    data_source(ds)
{
    ui->setupUi(this);
    ui->controlLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    chart = new BandwidthChart(GIGA, BITS);
    chart->addSeries(&bandwidth_series);
    bandwidth_series->setName("Bandwidth");

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(&chart_view);
}

LinuxPktgenWidget::~LinuxPktgenWidget() {
    delete ui;
}

void LinuxPktgenWidget::changeEvent(QEvent *e) {
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
#include "linuxpktgenwidget.h"
#include "ui_linuxpktgenwidget.h"

LinuxPktgenWidget::LinuxPktgenWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LinuxPktgenWidget),
    data_source(ds),
    thread(nullptr)
{
    ui->setupUi(this);
    ui->controlLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    ui->protocolComboBox->addItems(LinuxPktgen::getAvailableProtocols());
    ui->interfaceComboBox->addItems(LinuxPktgen::getAvailableInterfaces());
    ui->interfaceComboBox->setCurrentText(default_args["interface"]);
    ui->ipLineEdit->setText(default_args["address"]);
    ui->portSpinBox->setValue(default_args["port"].toInt());
    ui->macLineEdit->setText(default_args["mac"]);
    ui->threadsSpinBox->setValue(default_args["threads"].toInt());
    ui->dataSizeSpinBox->setValue(100000);
    ui->packetsCountSpinBox->setValue(5000000);
    ui->iterPacketsSpinBox->setValue(10000);

    chart = new BandwidthChart(GIGA, BITS);
    chart->addSeries(&bandwidth_series);
    bandwidth_series->setName("Bandwidth");

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->chartLayout->addWidget(&chart_view);

    start_stop = new StartStopWidget();
    ui->startStopLayout->addWidget(start_stop);
    QObject::connect(start_stop, &StartStopWidget::start, this, &LinuxPktgenWidget::onStart);
    QObject::connect(start_stop, &StartStopWidget::stop, this, &LinuxPktgenWidget::onStop);
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

void LinuxPktgenWidget::onStart() {

    chart->clear();

    QString protocol = ui->protocolComboBox->currentText();
    QString interface = ui->interfaceComboBox->currentText();
    QString ip_address = ui->ipLineEdit->text();
    const unsigned int port = ui->portSpinBox->value();
    QString mac_address = ui->macLineEdit->text();
    const quint64 threads = ui->threadsSpinBox->value();
    const quint64 data_size = ui->dataSizeSpinBox->value();
    const quint64 packets_count = ui->packetsCountSpinBox->value();
    const quint64 iter_packets_count = ui->iterPacketsSpinBox->value();

    auto pktgen = LinuxPktgen(protocol, interface, ip_address, port, mac_address, threads, data_size, iter_packets_count);
    thread = new LinuxPktgenThread(pktgen, packets_count, iter_packets_count);
    QObject::connect(thread, &LinuxPktgenThread::onBandwidthResult, this, &LinuxPktgenWidget::onBandwidthResult);
    QObject::connect(thread, &LinuxPktgenThread::finished, this, &LinuxPktgenWidget::onThreadFinished);
    thread->start();

}

void LinuxPktgenWidget::onBandwidthResult(BandwidthResult result) {
    bandwidth_series->append(result);
}

void LinuxPktgenWidget::onThreadFinished() {

    delete thread;
    thread = nullptr;
    start_stop->setStartState();

}

void LinuxPktgenWidget::onStop() {

    thread->requestInterruption();

}

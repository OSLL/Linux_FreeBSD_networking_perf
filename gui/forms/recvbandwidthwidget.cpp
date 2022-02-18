#include "recvbandwidthwidget.h"
#include "ui_recvbandwidthwidget.h"

RecvBandwidthWidget::RecvBandwidthWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvBandwidthWidget),
    start_stop(new StartStopWidget),
    data_source(ds),
    receiver_thread(nullptr)
{
    ui->setupUi(this);

    ui->protocolComboBox->addItems(Socket::getSupportedProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);

    ui->portSpinBox->setValue(default_args["port"].toInt());
    ui->threadsSpinBox->setValue(default_args["threads"].toInt());

    ui->prefixComboBox->addItems(units_prefixes_enum.allStrings());
    ui->prefixComboBox->setCurrentText(default_args["prefix"]);

    ui->unitComboBox->addItems(bandwidth_units_enum.allStrings());
    ui->prefixComboBox->setCurrentText(default_args["unit"]);

    chart = new BandwidthChart(GIGA, BYTES);
    chart->addSeries(&bandwidth_series);
    bandwidth_series->setName("Bandwidth");

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->graphicLayout->addWidget(&chart_view);

    QObject::connect(&start_stop, &StartStopWidget::start, this, &RecvBandwidthWidget::onStartClicked);
    QObject::connect(&start_stop, &StartStopWidget::stop, this, &RecvBandwidthWidget::onStopClicked);
    ui->controlLayout->addWidget(&start_stop);
}

RecvBandwidthWidget::~RecvBandwidthWidget()
{
    delete ui;
}

void RecvBandwidthWidget::changeEvent(QEvent *e)
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

//TODO: отчистка графика
void RecvBandwidthWidget::onStartClicked() {

    chart->clear();

    auto protocol = ui->protocolComboBox->currentText();
    auto port = ui->portSpinBox->value();
    auto threads_count = ui->threadsSpinBox->value();
    auto cpu_affinity = ui->affinityCheckBox->checkState() == Qt::CheckState::Checked;

    auto prefix = units_prefixes_enum.fromString(ui->prefixComboBox->currentText()).value();
    auto unit = bandwidth_units_enum.fromString(ui->unitComboBox->currentText()).value();

    chart->setPrefix(prefix);
    chart->setUnit(unit);

    auto sock = std::make_unique<Socket>(protocol);

    if (sock->bindToAny(port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: bind failed"));
    }

    receiver_thread = new BandwidthReceiverThread(std::move(sock), threads_count, cpu_affinity);
    QObject::connect(receiver_thread, &BandwidthReceiverThread::bandwidth, this, &RecvBandwidthWidget::onBandwidth);
    QObject::connect(receiver_thread, &BandwidthReceiverThread::finished, this, &RecvBandwidthWidget::onFinished);
    receiver_thread->start();
}

void RecvBandwidthWidget::onStopClicked() {

    receiver_thread->requestInterruption();

}

void RecvBandwidthWidget::onBandwidth(BandwidthResult result) {

    bandwidth_series->append(result);

}

void RecvBandwidthWidget::onFinished() {

    delete receiver_thread;
    receiver_thread = nullptr;
    start_stop.setStartState();

}



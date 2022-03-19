#include "sendbandwidthwidget.h"
#include "ui_sendbandwidthwidget.h"
#include "../../utils/default_args.h"

SendBandwidthWidget::SendBandwidthWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendBandwidthWidget),
    data_source(ds)
{
    ui->setupUi(this);

    ui->protocolComboBox->addItems(Socket::getSupportedProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);

    ui->ipEdit->setText(default_args["address"]);
    ui->portSpinBox->setValue(default_args["port"].toInt());
    ui->threadsSpinBox->setValue(default_args["threads"].toInt());

    ui->prefixComboBox->addItems(units_prefixes_enum.allStrings());
    ui->prefixComboBox->setCurrentText(default_args["prefix"]);

    ui->unitComboBox->addItems(bandwidth_units_enum.allStrings());
    ui->unitComboBox->setCurrentText(default_args["unit"]);

    ui->fileLineEdit->setText(default_args["data"]);
    ui->dataSizeSpinBox->setValue(default_args["data-size"].toInt());
    ui->durationSpinBox->setValue(default_args["duration"].toInt());

    ui->zeroCopyCheckBox->setCheckState(Qt::CheckState::Unchecked);
    ui->affinityCheckBox->setCheckState(Qt::CheckState::Unchecked);

    chart = new BandwidthChart(GIGA, BITS);
    chart->addSeries(&bandwidth_series);
    bandwidth_series->setName("Bandwidth");

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->resultLayout->addWidget(&chart_view);

    QObject::connect(&start_stop, &StartStopWidget::start, this, &SendBandwidthWidget::onStartClicked);
    QObject::connect(&start_stop, &StartStopWidget::stop, this, &SendBandwidthWidget::onStopClicked);
    ui->controlLayout->addWidget(&start_stop);
}

SendBandwidthWidget::~SendBandwidthWidget()
{
    delete ui;
}

void SendBandwidthWidget::changeEvent(QEvent *e)
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

void SendBandwidthWidget::onStartClicked() {

    chart->clear();

    auto protocol = ui->protocolComboBox->currentText();
    const QString ip_addr = ui->ipEdit->text();
    auto port = ui->portSpinBox->value();
    auto threads_count = ui->threadsSpinBox->value();

    auto prefix = units_prefixes_enum.fromString(ui->prefixComboBox->currentText()).value();
    auto unit = bandwidth_units_enum.fromString(ui->unitComboBox->currentText()).value();

    const QString filename = ui->fileLineEdit->text();
    quint64 data_size = ui->dataSizeSpinBox->value();
    const auto duration = ui->durationSpinBox->value();

    const bool zero_copy = ui->zeroCopyCheckBox->checkState() == Qt::CheckState::Checked;
    const bool cpu_affinity = ui->affinityCheckBox->checkState() == Qt::CheckState::Checked;

    chart->setPrefix(prefix);
    chart->setUnit(unit);

    auto current = QDateTime::currentDateTime();
    chart->getXAxis()->setRange(current, current.addSecs(duration));

    auto o_file = get_file(filename, data_size ? data_size : DEFAULT_NOT_ZERO_DATASIZE);
    if (!o_file) {
        QMessageBox::warning(this, tr("LFNP"), tr(("Error: can't open " + filename).toLocal8Bit().data()));
        start_stop.setStartState();
        return;
    }
    auto file = std::move(*o_file);

    if (!data_size) {
        data_size = file->size();
    }

    auto sock = std::make_unique<Socket>(protocol);

    if (sock->connectTo(ip_addr, port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: Can't connect"));
        start_stop.setStartState();
        return;
    }

    sender_thread = new BandwidthSenderThread(std::move(sock), threads_count, cpu_affinity, zero_copy, duration, data_size,
                                              ip_addr, port, std::move(file));
    QObject::connect(sender_thread, &BandwidthSenderThread::bandwidth, this, &SendBandwidthWidget::onBandwidth);
    QObject::connect(sender_thread, &BandwidthSenderThread::finished, this, &SendBandwidthWidget::onFinished);
    sender_thread->start();
}

void SendBandwidthWidget::onStopClicked() {

    sender_thread->terminate();

}

void SendBandwidthWidget::onBandwidth(BandwidthResult result) {

    bandwidth_series->append(result);

}

void SendBandwidthWidget::onFinished() {

    delete sender_thread;
    sender_thread = nullptr;
    start_stop.setStartState();

}
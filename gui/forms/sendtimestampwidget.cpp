#include <QMessageBox>
#include "sendtimestampwidget.h"
#include "ui_sendtimestampwidget.h"

SendTimestampWidget::SendTimestampWidget(BaseDataSource *ds, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SendTimestampWidget),
        data_source(ds),
        chart(nullptr),
        sender_thread(nullptr),
        software_series(nullptr),
        hardware_series(nullptr),
        in_call_series(nullptr)
{
    ui->setupUi(this);

    ui->protocolComboBox->addItems(Socket::getSupportedProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);

    ui->ipEdit->setText(default_args["address"]);
    ui->portSpinBox->setValue(default_args["port"].toInt());
    ui->packetsCountComboBox->setValue(default_args["packets-count"].toInt());

    ui->measureTypeComboBox->addItems(measure_type_enum.allStrings());
    ui->measureTypeComboBox->setCurrentText(default_args["measure-type"]);

    ui->fileLineEdit->setText(default_args["data"]);
    ui->dataSizeSpinBox->setValue(default_args["data-size"].toInt());

    ui->accuracyComboBox->addItems({"us", "ns"});
    ui->accuracyComboBox->setCurrentText("us");

    ui->delaySpinBox->setValue(default_args["delay"].toInt());
    ui->zeroCopyCheckBox->setCheckState(Qt::CheckState::Unchecked);

    QObject::connect(&start_stop, &StartStopWidget::start, this, &SendTimestampWidget::onStartClicked);
    QObject::connect(&start_stop, &StartStopWidget::stop, this, &SendTimestampWidget::onStopClicked);
    ui->controlWidget->layout()->addWidget(&start_stop);

    chart = new TimestampsChart(default_args["packets-count"].toInt(), true);
    chart_view.setChart(chart);

    chart->addSeries(&software_series);
    chart->addSeries(&hardware_series);
    chart->addSeries(&in_call_series);

    software_series->setName(tr("Software"));
    hardware_series->setName(tr("Hardware"));
    in_call_series->setName(tr("In call"));

    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->resultLayout->addWidget(&chart_view);
}

SendTimestampWidget::~SendTimestampWidget()
{
    delete ui;
}

void SendTimestampWidget::changeEvent(QEvent *e)
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

void SendTimestampWidget::onStartClicked() {

    const QString protocol = ui->protocolComboBox->currentText();
    const QString ip_addr = ui->ipEdit->text();
    const int port = ui->portSpinBox->value();
    const int packets_count = ui->packetsCountComboBox->value();
    const MeasureType measure_type = measure_type_enum.fromString(ui->measureTypeComboBox->currentText()).value();
    const QString filename = ui->fileLineEdit->text();
    quint64 data_size = ui->dataSizeSpinBox->value();
    quint64 delay = ui->delaySpinBox->value();
    const bool is_us = ui->accuracyComboBox->currentText() == "us";
    const bool zero_copy = ui->zeroCopyCheckBox->checkState() == Qt::CheckState::Checked;

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

    auto *sock = new Socket("tcp");
    data_source->setSendSockOpt(*sock, measure_type);

    if (sock->connectTo(ip_addr, port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: Can't connect"));
        start_stop.setStartState();
        return;
    }

    recreateChart(packets_count, is_us);

    sender_thread = new TimestampsSenderThread(sock, protocol, file, data_size, zero_copy, data_source, packets_count, delay, measure_type);

    QObject::connect(sender_thread, &TimestampsSenderThread::packetSent, this, &SendTimestampWidget::onPacketSent);
    QObject::connect(sender_thread, &TimestampsSenderThread::finished, this, &SendTimestampWidget::onThreadFinished);
    sender_thread->start();

}

void SendTimestampWidget::recreateChart(quint64 packets_count, bool is_us) {

    chart->clear();
    chart->setPacketsCount(packets_count);
    chart->setUS(is_us);

}

void SendTimestampWidget::onPacketSent(std::optional<SendTimestamp> o_timestamp) {

    if (o_timestamp) {
        auto timestamp = o_timestamp.value();

        if (timestamp.software_send) { software_series->append(*timestamp.getSoftwareTime()); };
        if (timestamp.hardware_send) { hardware_series->append(*timestamp.getHardwareTime()); }
        in_call_series->append(timestamp.getInCallTime());

    }
}

void SendTimestampWidget::onStopClicked() {

    sender_thread->requestInterruption();
    sender_thread->wait();

}

void SendTimestampWidget::onThreadFinished() {

    start_stop.setStartState();

    delete sender_thread;
    sender_thread = nullptr;

}

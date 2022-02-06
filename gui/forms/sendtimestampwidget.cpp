#include <QMessageBox>
#include "sendtimestampwidget.h"
#include "ui_sendtimestampwidget.h"

SendTimestampWidget::SendTimestampWidget(BaseDataSource *ds, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SendTimestampWidget),
        data_source(ds),
        chart(nullptr),
        start_stop(new StartStopWidget),
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

    QObject::connect(start_stop, &StartStopWidget::start, this, &SendTimestampWidget::onStartClicked);
    QObject::connect(start_stop, &StartStopWidget::stop, this, &SendTimestampWidget::onStopClicked);
    ui->controlWidget->layout()->addWidget(start_stop);

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
        return;
    }
    auto file = std::move(*o_file);

    if (!data_size) {
        data_size = file->size();
    }

    auto *sock = new Socket(protocol);
    data_source->setSendSockOpt(*sock, measure_type);

    if (sock->connectTo(ip_addr, port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: Can't connect"));
        return;
    }

    SendProcessFunc send_func = std::bind(&BaseDataSource::processSendTimestamp, data_source, _1, _2, _3);

    recreateChart(packets_count, is_us);

    sender_thread = new TimestampsSenderThread(sock, file, data_size, zero_copy, send_func, packets_count, delay);

    QObject::connect(sender_thread, &TimestampsSenderThread::packetSent, this, &SendTimestampWidget::onPacketSent);
    QObject::connect(sender_thread, &TimestampsSenderThread::finished, this, &SendTimestampWidget::onThreadFinished);
    sender_thread->start();

}

void SendTimestampWidget::recreateChart(quint64 packets_count, bool is_us) {

    auto new_chart = new TimestampsChart(packets_count, is_us);
    chart_view.setChart(new_chart);
    delete chart;
    chart = new_chart;

    chart->addSeries(reinterpret_cast<QXYSeries **>(&software_series));
    chart->addSeries(reinterpret_cast<QXYSeries **>(&hardware_series));
    chart->addSeries(reinterpret_cast<QXYSeries **>(&in_call_series));

    software_series->setName("Software");
    hardware_series->setName("Hardware");
    in_call_series->setName("In call");
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

    start_stop->setStartState();

    delete sender_thread;
    sender_thread = nullptr;

}

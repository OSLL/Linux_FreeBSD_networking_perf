#include "recvtimestampwidget.h"
#include "ui_recvtimestampwidget.h"

RecvTimestampWidget::RecvTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvTimestampWidget),
    data_source(ds),
    receiver_thread(nullptr)
{
    ui->setupUi(this);

    ui->protocolComboBox->addItems(Socket::getSupportedProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);

    ui->portSpinBox->setValue(default_args["port"].toInt());
    ui->packetsCountComboBox->setValue(default_args["packets-count"].toInt());

    ui->accuracyComboBox->addItems({"us", "ns"});
    ui->accuracyComboBox->setCurrentText("us");

    chart = new TimestampsChart(default_args["packets-count"].toInt(), true);
    chart->addSeries(&software_series);
    chart->addSeries(&hardware_series);
    chart->addSeries(&in_call_series);
    chart->addSeries(&total_series);

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->resultLayout->addWidget(&chart_view);

    QObject::connect(&start_stop, &StartStopWidget::start, this, &RecvTimestampWidget::onStartClicked);
    QObject::connect(&start_stop, &StartStopWidget::stop, this, &RecvTimestampWidget::onStopClicked);
    ui->startStopLayout->addWidget(&start_stop);
}

RecvTimestampWidget::~RecvTimestampWidget()
{
    delete receiver_thread;
    delete ui;
}

void RecvTimestampWidget::changeEvent(QEvent *e)
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

void RecvTimestampWidget::onStartClicked() {

    const QString protocol = ui->protocolComboBox->currentText();
    const int port = ui->portSpinBox->value();
    const int packets_count = ui->packetsCountComboBox->value();
    const bool is_us = ui->accuracyComboBox->currentText() == "us";

    recreateChart(packets_count, is_us);

    Socket *sock = new Socket(protocol);
    data_source->setRecvSockOpt(*sock);

    if (sock->bindToAny(port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: bind failed"));
        return;
    }

    RecvProcessFunc recv_func = std::bind(&BaseDataSource::processRecvTimestamp, data_source, _1, _2, _3, _4);
    receiver_thread = new TimestampsReceiverThread(sock, recv_func, packets_count);

    QObject::connect(receiver_thread, &TimestampsReceiverThread::packetReceived, this, &RecvTimestampWidget::onPacketReceived);
    QObject::connect(receiver_thread, &TimestampsReceiverThread::finished, this, &RecvTimestampWidget::onThreadFinished);
    receiver_thread->start();
}

void RecvTimestampWidget::onPacketReceived(const ReceiveTimestamp recv_ts) {

    if (recv_ts.software_recv) { software_series->append(*recv_ts.getSoftwareTime()); };
    if (recv_ts.hardware_recv) { hardware_series->append(*recv_ts.getHardwareTime()); }
    in_call_series->append(recv_ts.getInCallTime());
    total_series->append(recv_ts.getTotalTime());

}

void RecvTimestampWidget::recreateChart(quint64 packets_count, bool is_us) {

    chart->clear();
    chart->setPacketsCount(packets_count);
    chart->setUS(is_us);

}

void RecvTimestampWidget::onStopClicked() {

    receiver_thread->terminate();
    receiver_thread->wait();

}

void RecvTimestampWidget::onThreadFinished() {

    start_stop.setStartState();

    delete receiver_thread;
    receiver_thread = nullptr;

}

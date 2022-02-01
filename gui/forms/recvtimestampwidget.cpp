#include "recvtimestampwidget.h"
#include "ui_recvtimestampwidget.h"

RecvTimestampWidget::RecvTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvTimestampWidget),
    data_source(ds),
    receiver_thread(nullptr)
{
    ui->setupUi(this);
    ui->stopButton->setDisabled(true);

    ui->protocolComboBox->addItems(Socket::getSupportedProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);

    ui->portSpinBox->setValue(default_args["port"].toInt());
    ui->packetsCountComboBox->setValue(default_args["packets-count"].toInt());

    ui->accuracyComboBox->addItems({"us", "ns"});
    ui->accuracyComboBox->setCurrentText("us");

    QObject::connect(ui->startButton, &QPushButton::clicked, this, &RecvTimestampWidget::onStartClicked);
    QObject::connect(ui->stopButton, &QPushButton::clicked, this, &RecvTimestampWidget::onStopClicked);

    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->resultLayout->addWidget(&chart_view);
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

    recreateChart(is_us);

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

    ui->startButton->setDisabled(true);
    ui->stopButton->setDisabled(false);
}

void RecvTimestampWidget::onPacketReceived(const ReceiveTimestamp recv_ts) {

    if (recv_ts.software_recv) { software_series->append(*recv_ts.getSoftwareTime()); };
    if (recv_ts.hardware_recv) { hardware_series->append(*recv_ts.getHardwareTime()); }
    in_call_series->append(recv_ts.getInCallTime());
    total_series->append(recv_ts.getTotalTime());

}

void RecvTimestampWidget::recreateChart(bool is_us) {

    auto prev_chart = chart_view.chart();
    auto chart = new DynamicYAxisChart();
    chart_view.setChart(chart);
    delete prev_chart;

    auto x_axis = new QValueAxis();
    x_axis->setRange(0, 100);
    chart_view.chart()->addAxis(x_axis, Qt::AlignBottom);

    if (is_us) {

        std::function<qreal(quint64)> to_us = [](quint64 ns_val) {return ns_val/1000;};

        software_series = new FuncSeries<TimeSeries<QLineSeries>, qreal, quint64>(to_us);
        hardware_series = new FuncSeries<TimeSeries<QLineSeries>, qreal, quint64>(to_us);
        in_call_series = new FuncSeries<TimeSeries<QLineSeries>, qreal, quint64>(to_us);
        total_series = new FuncSeries<TimeSeries<QLineSeries>, qreal, quint64>(to_us);

    } else {

        software_series = new TimeSeries<QLineSeries>();
        hardware_series = new TimeSeries<QLineSeries>();
        in_call_series = new TimeSeries<QLineSeries>();
        total_series = new TimeSeries<QLineSeries>();

    }

    software_series->setName("Software");
    hardware_series->setName("Hardware");
    in_call_series->setName("In call");
    total_series->setName("Total");

    chart->addSeries(software_series);
    chart->addSeries(hardware_series);
    chart->addSeries(in_call_series);
    chart->addSeries(total_series);

    software_series->attachAxis(x_axis);
    hardware_series->attachAxis(x_axis);
    in_call_series->attachAxis(x_axis);
    total_series->attachAxis(x_axis);
}

void RecvTimestampWidget::onStopClicked() {

    receiver_thread->terminate();
    receiver_thread->wait();

}

void RecvTimestampWidget::onThreadFinished() {

    ui->startButton->setDisabled(false);
    ui->stopButton->setDisabled(true);

    delete receiver_thread;
    receiver_thread = nullptr;

}

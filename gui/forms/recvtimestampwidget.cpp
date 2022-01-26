#include "recvtimestampwidget.h"
#include "ui_recvtimestampwidget.h"
#include "../../utils/default_args.h"
#include "../../types/enums/MeasureType.h"

RecvTimestampWidget::RecvTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvTimestampWidget),
    data_source(ds),
    software_series(new TimeSeries<QLineSeries>()),
    hardware_series(new TimeSeries<QLineSeries>()),
    in_call_series(new TimeSeries<QLineSeries>()),
    total_series(new TimeSeries<QLineSeries>()),
    receiver_thread(nullptr)
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

    QObject::connect(ui->startButton, &QPushButton::clicked, this, &RecvTimestampWidget::onStartClicked);

    software_series->setName("Software");
    hardware_series->setName("Hardware");
    in_call_series->setName("In call");
    total_series->setName("Total");

    auto x_axis = new QValueAxis();
    x_axis->setRange(0, 100);

    auto chart = new DynamicYAxisChart();
    chart->addAxis(x_axis, Qt::AlignBottom);

    chart->addSeries(software_series);
    chart->addSeries(hardware_series);
    chart->addSeries(in_call_series);
    chart->addSeries(total_series);

    chart_view.setChart(chart);
    chart_view.setRenderHint(QPainter::Antialiasing);
    ui->resultLayout->addWidget(&chart_view);
}

RecvTimestampWidget::~RecvTimestampWidget()
{
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
    const QString ip_addr = ui->ipEdit->text();
    const int port = ui->portSpinBox->value();
    const int packets_count = ui->packetsCountComboBox->value();
    const MeasureType measure_type = measure_type_enum.fromString(ui->measureTypeComboBox->currentText()).value();
    const QString filename = ui->fileLineEdit->text();
    const int data_size = ui->dataSizeSpinBox->value();
    const bool is_us = ui->accuracyComboBox->currentText() == "us";

    Socket *sock = new Socket(protocol);
    data_source->setRecvSockOpt(*sock);

    if (sock->bindToAny(port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: bind failed"));
        return;
    }

    RecvProcessFunc recv_func = std::bind(&BaseDataSource::processRecvTimestamp, data_source, _1, _2, _3, _4);
    receiver_thread = new TimestampsReceiverThread(sock, recv_func, packets_count);

    QObject::connect(receiver_thread, &TimestampsReceiverThread::packetReceived, this, &RecvTimestampWidget::onPacketReceived);

    receiver_thread->start();
}

void RecvTimestampWidget::onPacketReceived(const InSystemTimeInfo &time_info) {

    if (!time_info.software_time.empty()) {software_series->append(time_info.software_time.last());}
    if (!time_info.hardware_time.empty()) {hardware_series->append(time_info.hardware_time.last());}
    if (!time_info.in_call_time.empty()) {in_call_series->append(time_info.in_call_time.last());}
    if (!time_info.total_time.empty()) {total_series->append(time_info.total_time.last());}

}

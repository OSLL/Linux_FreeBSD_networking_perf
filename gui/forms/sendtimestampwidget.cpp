#include <QMessageBox>
#include "sendtimestampwidget.h"
#include "ui_sendtimestampwidget.h"

SendTimestampWidget::SendTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendTimestampWidget),
    data_source(ds)
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

    QObject::connect(ui->startButton, &QPushButton::clicked, this, &SendTimestampWidget::onStartClicked);
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
    int data_size = ui->dataSizeSpinBox->value();
    const bool is_us = ui->accuracyComboBox->currentText() == "us";
    const bool zero_copy = ui->zeroCopyCheckBox->checkState() == Qt::CheckState::Checked;

    auto *file = new QFile(filename);

    if (!file->open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, tr("LFNP"), tr(("Error: can't open " + filename).toLocal8Bit().data()));
        return;
    }

    if (!data_size) {
        if (filename == default_args["data"]) {
            data_size = 1000;
        } else {
            data_size = file->size();
        }
    }

    auto *sock = new Socket(protocol);
    data_source->setSendSockOpt(*sock, measure_type);

    if (sock->connectTo(ip_addr, port) < 0) {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: Can't connect"));
        return;
    }

    SendProcessFunc send_func = std::bind(&BaseDataSource::processSendTimestamp, data_source, _1, _2, _3);

    sender = new TimestampsSenderThread(sock, file, data_size, zero_copy, send_func, packets_count);
    sender->start();

    ui->startButton->setDisabled(true);
}

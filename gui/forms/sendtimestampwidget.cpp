#include "sendtimestampwidget.h"
#include "ui_sendtimestampwidget.h"

SendTimestampWidget::SendTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SendTimestampWidget)
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
    const int data_size = ui->dataSizeSpinBox->value();
    const bool is_us = ui->accuracyComboBox->currentText() == "us";

}

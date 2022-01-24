#include "recvtimestampwidget.h"
#include "ui_recvtimestampwidget.h"
#include "../../utils/default_args.h"
#include "../../types/enums/MeasureType.h"

RecvTimestampWidget::RecvTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvTimestampWidget),
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

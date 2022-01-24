#include "recvtimestampwidget.h"
#include "ui_recvtimestampwidget.h"

RecvTimestampWidget::RecvTimestampWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvTimestampWidget),
    data_source(ds)
{
    ui->setupUi(this);

    ui->protocolComboBox->addItems(Socket::getSupportedProtocols());
    ui->protocolComboBox->setCurrentText("tcp");
    ui->ipEdit->setText("127.0.0.1");
    ui->portSpinBox->setValue(7435);
    ui->packetsCountComboBox->setValue(100);
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

#include "recvbandwidthwidget.h"
#include "ui_recvbandwidthwidget.h"

RecvBandwidthWidget::RecvBandwidthWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RecvBandwidthWidget)
{
    ui->setupUi(this);
}

RecvBandwidthWidget::~RecvBandwidthWidget()
{
    delete ui;
}

void RecvBandwidthWidget::changeEvent(QEvent *e)
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

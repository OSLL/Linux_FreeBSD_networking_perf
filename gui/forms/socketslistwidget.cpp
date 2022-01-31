#include "socketslistwidget.h"
#include "ui_socketslistwidget.h"

SocketsListWidget::SocketsListWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SocketsListWidget),
    data_source(ds),
    table(new QTableView),
    model(new SocketsListModel),
    update_timer(new QTimer)
{
    ui->setupUi(this);
    ui->controllLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

    ui->protocolComboBox->addItems(data_source->getSupportedSocketsListProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);
    protocol = default_args["protocol"];

    updateSocketsList();

    table->setModel(model);
    table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableLayout->addWidget(table);
    update_timer->setInterval(1000);
    update_timer->start();

    QObject::connect(ui->protocolComboBox, &QComboBox::currentTextChanged, this, &SocketsListWidget::onProtocolChanged);
    QObject::connect(update_timer, &QTimer::timeout, this, &SocketsListWidget::onTimeout);
}

SocketsListWidget::~SocketsListWidget()
{
    delete ui;
}

void SocketsListWidget::changeEvent(QEvent *e)
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

void SocketsListWidget::onProtocolChanged(const QString &_protocol) {
    protocol = _protocol;
    updateSocketsList();
}

void SocketsListWidget::updateSocketsList() {

    auto sockets = data_source->getSockets(protocol);
    model->setData(sockets);

}

void SocketsListWidget::onTimeout() {
    updateSocketsList();
}

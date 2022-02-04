#include "socketslistwidget.h"
#include "ui_socketslistwidget.h"
#include "trackvaluewidget.h"

SocketsListWidget::SocketsListWidget(BaseDataSource *ds, MainTabWidget *_tab_widget, QWidget *parent) :
        QWidget(parent),
        ui(new Ui::SocketsListWidget),
        data_source(ds),
        table_view(new QTableView),
        model(new SocketsListModel),
        update_timer(new QTimer),
        tab_widget(_tab_widget),
        menu(table_view)
{
    ui->setupUi(this);
    ui->controllLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

    ui->protocolComboBox->addItems(data_source->getSupportedSocketsListProtocols());
    ui->protocolComboBox->setCurrentText(default_args["protocol"]);
    protocol = default_args["protocol"];

    updateSocketsList();

    table_view->setModel(model);
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    ui->tableLayout->addWidget(table_view);
    update_timer->setInterval(1000);
    update_timer->start();

    QObject::connect(ui->protocolComboBox, &QComboBox::currentTextChanged, this, &SocketsListWidget::onProtocolChanged);
    QObject::connect(update_timer, &QTimer::timeout, this, &SocketsListWidget::onTimeout);
    menu.addAction("Track in new tab", this,&SocketsListWidget::onTrackInTab);
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

void SocketsListWidget::onTrackInTab() {

    auto index = menu.getIndex();
    auto column = index.column();

    SocketInfo socket = model->dataByRow(index.row());

    // Лучше скопировать по значению, так как при изменении протокола в this, не будет найден сокет
    auto _data_source = data_source;
    QString _protocol = this->protocol;

    // В зависимости от того, на какую клетку нажали, нужно отслеживать разные параметры сокета
    QPair<QString, std::function<int(SocketInfo&)>> column_args;
    if (column <= 4) {
        column_args = {"TX Queue", [](SocketInfo &socket) {return socket.tx_queue_size;}};
    } else if (column == 5) {
        column_args = {"RX Queue", [](SocketInfo &socket) {return socket.rx_queue_size;}};
    } else if (column == 6) {
        column_args = {"Refs", [](SocketInfo &socket) {return socket.ref;}};
    } else if (column == 7) {
        column_args = {"Drops", [](SocketInfo &socket) {return socket.drops;}};
    }

    int tab_index = tab_widget->addTab(new TrackValueWidget(
            [_data_source, _protocol, socket, column_args]() {

            auto new_socket = _data_source->getOneSocket(_protocol,
                                                         socket.local_address, socket.local_port,
                                                         socket.foreign_address,socket.foreign_port);
            if (new_socket) {
                return column_args.second(new_socket.value());
            } else {
                return 0;
            }

        }, column_args.first),
                       column_args.first + " (" +
                       QString::number(socket.local_port) + "-" + QString::number(socket.foreign_port) + ")");

    tab_widget->setCurrentIndex(tab_index);
}

#include "protocolsstatswidget.h"
#include "ui_protocolsstatswidget.h"
#include "../tables/TableContextMenu.h"
#include "trackvaluewidget.h"

ProtocolsStatsWidget::ProtocolsStatsWidget(BaseDataSource *ds, MainTabWidget *_tab_widget, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProtocolsStatsWidget),
    data_source(ds),
    table_view(new QTableView),
    tab_widget(_tab_widget),
    stats_model(new ProtocolStatsModel),
    update_timer(std::make_unique<QTimer>()),
    menu(table_view)
{
    ui->setupUi(this);
    ui->controlLayout->setAlignment(Qt::AlignmentFlag::AlignLeft);

    ui->protocolComboBox->addItems(data_source->getSupportedStatsProtocols());

    protocol = default_args["protocol"];
    ui->protocolComboBox->setCurrentText(protocol);

    updateStats();

    table_view->setModel(stats_model);
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableLayout->addWidget(table_view);

    update_timer->setInterval(1000);
    QObject::connect(update_timer.get(), &QTimer::timeout, this, &ProtocolsStatsWidget::onTimeout);
    QObject::connect(ui->protocolComboBox, &QComboBox::currentTextChanged, this, &ProtocolsStatsWidget::onProtocolChanged);
    update_timer->start();

    menu.addAction("Track in new tab", this,&ProtocolsStatsWidget::onTrackInTab);
}

ProtocolsStatsWidget::~ProtocolsStatsWidget()
{
    delete ui;
    delete table_view;
    delete stats_model;
}

void ProtocolsStatsWidget::changeEvent(QEvent *e)
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

void ProtocolsStatsWidget::updateStats() {

    auto stats = data_source->getProtocolStats(protocol);
    if (stats) {
        stats_model->setData(stats.value());
    } else {
        QMessageBox::warning(this, tr("LFNP"), tr("Error: can't get protocol stats"));
    }

}

void ProtocolsStatsWidget::onProtocolChanged(const QString &_protocol) {

    protocol = _protocol;
    updateStats();

}

void ProtocolsStatsWidget::onTimeout() {

    updateStats();

}

void ProtocolsStatsWidget::onTrackInTab() {

    auto index = menu.getIndex();
    auto name_index = table_view->model()->index(index.row(), 0);
    auto prop_name = table_view->model()->data(name_index).toString();

    int tab_index = tab_widget->addTab(new TrackValueWidget(
            std::bind(&BaseDataSource::getOneStat, data_source, protocol, prop_name), prop_name),
                                       "Track " + prop_name);
    tab_widget->setCurrentIndex(tab_index);

}

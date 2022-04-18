#include "dropsinfowidget.h"
#include "ui_dropsinfowidget.h"

DropsInfoWidget::DropsInfoWidget(BaseDataSource *_ds, MainTabWidget *_tab_widget, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DropsInfoWidget),
    data_source(_ds),
    tab_widget(_tab_widget),
    table_view(new QTableView),
    menu(table_view)
{
    ui->setupUi(this);

    auto drops_data = data_source->getDropsInfo();
    drops_info_model = new DropsInfoModel(drops_data);

    table_view->setModel(drops_info_model);
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSpans();

    menu.addAction("Track in new tab", this, &DropsInfoWidget::onTrackInTab);
    menu.addColumnToFilter(0);

    ui->dropsInfoLayout->addWidget(table_view);

    QObject::connect(&timer, &QTimer::timeout, this, &DropsInfoWidget::onTimeout);
    timer.setInterval(std::chrono::seconds(1));
    timer.start();
}

DropsInfoWidget::~DropsInfoWidget()
{
    delete ui;
}

void DropsInfoWidget::changeEvent(QEvent *e)
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

void DropsInfoWidget::setSpans() {
    table_view->clearSpans();
    for (int row = 0; row < table_view->model()->rowCount(); row++) {
        for (int col = 0; col < table_view->model()->columnCount(); col++) {
            QSize span = table_view->model()->span(table_view->model()->index(row, col));
            if (!span.isEmpty()) {
                table_view->setSpan(row, col, span.height(), span.width());
            }
        }
    }
}

void DropsInfoWidget::onTimeout() {

    auto drops_data = data_source->getDropsInfo();
    drops_info_model->setData(drops_data);
    setSpans();

}

void DropsInfoWidget::onTrackInTab() {

    auto index = menu.getIndex();

    QString source = drops_info_model->sourceAt(index.row());
    auto source_drops_info = drops_info_model->getDrops(source);

    QString type;
    std::function<int(DropsInfo&)> get_value_func;
    if (source_drops_info->is_common) {
        type = "Drops";
        get_value_func = [](DropsInfo &drops) {return drops.common_drops;};
    } else {
        if (index.column() == 1) {
            type = "RX Drops";
            get_value_func = [](DropsInfo &drops) {return drops.rx_drops;};
        } else {
            type = "TX Drops";
            get_value_func = [](DropsInfo &drops) {return drops.tx_drops;};
        }
    }

    tab_widget->addTab(new TrackValueWidget([this, source, get_value_func]() {

        auto o_drops_info = drops_info_model->getDrops(source);
        if (o_drops_info) {
            return get_value_func(o_drops_info.value());
        }
        return 0;

        }, type), QString("%1 of %2").arg(type, source));

}

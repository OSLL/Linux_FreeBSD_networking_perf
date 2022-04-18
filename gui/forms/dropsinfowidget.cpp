#include "dropsinfowidget.h"
#include "ui_dropsinfowidget.h"

DropsInfoWidget::DropsInfoWidget(BaseDataSource *_ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DropsInfoWidget),
    data_source(_ds),
    table_view(new QTableView)
{
    ui->setupUi(this);

    auto drops_data = data_source->getDropsInfo();
    drops_info_model = new DropsInfoModel(drops_data);

    table_view->setModel(drops_info_model);
    table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setSpans();

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

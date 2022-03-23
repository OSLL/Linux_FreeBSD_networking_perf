#include "profilerflamewidget.h"
#include "ui_profilerflamewidget.h"
#include "../../types/FlameGraph.h"

ProfilerFlameWidget::ProfilerFlameWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilerFlameWidget)
{
    ui->setupUi(this);
    ui->horizontalLayout->addWidget(new FlameGraph(ds->getProfilerData().value()));
}

ProfilerFlameWidget::~ProfilerFlameWidget()
{
    delete ui;
}

void ProfilerFlameWidget::changeEvent(QEvent *e)
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

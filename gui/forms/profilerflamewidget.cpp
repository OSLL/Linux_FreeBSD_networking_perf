#include <QMessageBox>
#include "profilerflamewidget.h"
#include "ui_profilerflamewidget.h"

#define WAIT_SECONDS 10

ProfilerFlameWidget::ProfilerFlameWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilerFlameWidget),
    data_source(ds)
{
    collector = data_source->getProfilerCollector();
    ui->setupUi(this);

    progress_bar = new QProgressBar();
    ui->horizontalLayout->addWidget(progress_bar);

    progress_bar->setRange(0, WAIT_SECONDS);
    timer.setInterval(std::chrono::seconds(1));
    QObject::connect(&timer, &QTimer::timeout, this, &ProfilerFlameWidget::onTimer);

    collector->onStart();
    timer.start();
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

void ProfilerFlameWidget::onTimer() {
    progress_bar->setValue(progress_bar->value()+1);

    collector->onTimer();

    if (progress_bar->value() == progress_bar->maximum()) {
        parser = collector->onEnd();
        timer.stop();

        ui->horizontalLayout->removeWidget(progress_bar);
        ui->horizontalLayout->addWidget(new FlameGraph(parser));
    }
}

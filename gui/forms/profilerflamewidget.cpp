#include <QMessageBox>
#include "profilerflamewidget.h"
#include "ui_profilerflamewidget.h"
#include "../../utils/default_args.h"

ProfilerFlameWidget::ProfilerFlameWidget(BaseDataSource *ds, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilerFlameWidget),
    data_source(ds),
    flame_graph(nullptr),
    progress_bar(nullptr)
{
    ui->setupUi(this);
    ui->controlLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    flame_graph = new FlameGraph(QVector<FuncProfilerTreeNode*>());
    ui->flameGraphLayout->addWidget(flame_graph);

    ui->durationSpinBox->setValue(default_args["duration"].toInt());
    ui->CPUComboBox->setEnabled(false);
    ui->protocolComboBox->setEnabled(false);

    timer.setInterval(std::chrono::seconds(1));
    QObject::connect(&timer, &QTimer::timeout, this, &ProfilerFlameWidget::onTimer);
    QObject::connect(ui->CPUComboBox, &QComboBox::currentTextChanged, this, &ProfilerFlameWidget::CPUChanged);
    QObject::connect(ui->startButton, &QPushButton::clicked, this, &ProfilerFlameWidget::onStartClicked);
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
        profiler_data = collector->onEnd();
        timer.stop();
        ui->startButton->setEnabled(true);

        auto available_cpus = profiler_data.keys();
        for (const auto cpu: available_cpus) {
            ui->CPUComboBox->addItem(QString::number(cpu));
        }
        ui->CPUComboBox->setEnabled(true);
        ui->protocolComboBox->setEnabled(true);

        ui->flameGraphLayout->removeWidget(progress_bar);
        delete progress_bar;
        progress_bar = nullptr;

        flame_graph = new FlameGraph(profiler_data.value(ui->CPUComboBox->currentText().toInt()));
        ui->flameGraphLayout->addWidget(flame_graph);
    }
}

void ProfilerFlameWidget::CPUChanged(const QString& s_cpu) {
    if (flame_graph)
        flame_graph->setNodes(profiler_data.value(s_cpu.toInt()));
}

void ProfilerFlameWidget::onStartClicked() {

    ui->CPUComboBox->setEnabled(false);
    ui->protocolComboBox->setEnabled(false);

    const auto duration = ui->durationSpinBox->value();

    ui->flameGraphLayout->removeWidget(flame_graph);
    delete flame_graph;
    flame_graph = nullptr;

    progress_bar = new QProgressBar();
    progress_bar->setRange(0, duration);
    progress_bar->setValue(0);
    ui->flameGraphLayout->addWidget(progress_bar);

    collector = data_source->getProfilerCollector();
    collector->onStart();
    timer.start();
    ui->startButton->setEnabled(false);
}

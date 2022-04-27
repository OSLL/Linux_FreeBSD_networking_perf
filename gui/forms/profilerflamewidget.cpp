#include "profilerflamewidget.h"
#include "ui_profilerflamewidget.h"

ProfilerFlameWidget::ProfilerFlameWidget(BaseDataSource *ds, MainTabWidget *_tab_widget, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProfilerFlameWidget),
    data_source(ds),
    flame_graph(nullptr),
    progress_bar(nullptr),
    tab_widget(_tab_widget)
{
    ui->setupUi(this);
    ui->controlLayout->setAlignment(Qt::AlignmentFlag::AlignTop);

    flame_graph = new FlameGraph();
    ui->flameGraphLayout->addWidget(flame_graph);

    ui->durationSpinBox->setValue(default_args["duration"].toInt());
    ui->CPUComboBox->setEnabled(false);
    ui->PIDComboBox->setEnabled(false);
    ui->PIDComboBox->setDuplicatesEnabled(false);

    QStringList supported_protocols = ds->getProfilerCollector()->getSupportedProtocols();
    protocols_model = new ProfilerProtocolsModel(supported_protocols);
    ui->protocolComboBox->setEditable(true);
    ui->protocolComboBox->lineEdit()->setReadOnly(true);
    ui->protocolComboBox->setEditText("");
    ui->protocolComboBox->setModel(protocols_model);

    timer.setInterval(std::chrono::seconds(1));
    QObject::connect(&timer, &QTimer::timeout, this, &ProfilerFlameWidget::onTimer);
    QObject::connect(ui->CPUComboBox, &QComboBox::currentTextChanged, this, &ProfilerFlameWidget::CPUChanged);
    QObject::connect(ui->PIDComboBox, &QComboBox::currentTextChanged, this, &ProfilerFlameWidget::PIDChanged);
    QObject::connect(ui->startButton, &QPushButton::clicked, this, &ProfilerFlameWidget::onStartClicked);
    QObject::connect(ui->protocolComboBox, QOverload<int>::of(&QComboBox::activated), this, &ProfilerFlameWidget::onProtocolClicked);
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

        ui->PIDComboBox->clear();
        auto available_pids = profiler_data.keys();
        for (const auto pid: available_pids) {
            ui->PIDComboBox->addItem(QString::number(pid));
        }

        ui->CPUComboBox->setEnabled(true);
        ui->PIDComboBox->setEnabled(true);
        ui->protocolComboBox->setEnabled(true);

        ui->flameGraphLayout->removeWidget(progress_bar);
        delete progress_bar;
        progress_bar = nullptr;

        flame_graph = new FlameGraph(profiler_data
                                             .value(ui->PIDComboBox->currentText().toULongLong())
                                             .value(ui->CPUComboBox->currentText().toInt())
                );
        ui->flameGraphLayout->addWidget(flame_graph);
        QObject::connect(flame_graph, &FlameGraph::nodeClick, this, &ProfilerFlameWidget::onNodeClick);
    }
}

void ProfilerFlameWidget::onStartClicked() {

    ui->CPUComboBox->clear();
    ui->PIDComboBox->clear();

    ui->CPUComboBox->setEnabled(false);
    ui->PIDComboBox->setEnabled(false);
    ui->protocolComboBox->setEnabled(false);
    ui->startButton->setEnabled(false);

    const auto duration = ui->durationSpinBox->value();

    ui->flameGraphLayout->removeWidget(flame_graph);
    delete flame_graph;
    flame_graph = nullptr;

    progress_bar = new QProgressBar();
    progress_bar->setRange(0, duration);
    progress_bar->setValue(0);
    ui->flameGraphLayout->addWidget(progress_bar);

    collector = data_source->getProfilerCollector();
    collector->onStart(protocols_model->getChecked());
    timer.start();
}

void ProfilerFlameWidget::CPUChanged(const QString& s_cpu) {
    if (flame_graph) {
        flame_graph->setNodes(profiler_data
                                      .value(ui->PIDComboBox->currentText().toULongLong())
                                      .value(ui->CPUComboBox->currentText().toInt())
        );
    }
}

void ProfilerFlameWidget::PIDChanged(const QString &s_pid) {

    ui->CPUComboBox->clear();
    auto available_cpus = profiler_data[s_pid.toULongLong()].keys();
    for (const auto cpu: available_cpus) {
        ui->CPUComboBox->addItem(QString::number(cpu));
    }

}

void ProfilerFlameWidget::onProtocolClicked(int index) {
    protocols_model->check(index);
    ui->protocolComboBox->setEditText(protocols_model->displayText());
}

void ProfilerFlameWidget::onNodeClick(QMouseEvent *event, const FuncProfilerTreeNode *func_node) {

    QString func_name = func_node->getFuncName();
    QVector<FuncProfilerTreeNode *> func_nodes;
    for (const auto &cpu_nodes: profiler_data) {
        for (const auto &nodes: cpu_nodes) {
            for (const auto &node: nodes) {
                filterFuncNodes(node, func_name, func_nodes);
            }
        }
    }

    std::sort(func_nodes.begin(), func_nodes.end(), [](FuncProfilerTreeNode *a, FuncProfilerTreeNode *b) {
        return a->getRange().from - b->getRange().from;
    });


    timespec up;
    clock_gettime(CLOCK_BOOTTIME, &up);
    quint64 unix_up = QDateTime::currentMSecsSinceEpoch() - (up.tv_sec*1000+up.tv_nsec/1000000L);

    QVector<std::pair<quint64, quint64>> func_values;
    for (const auto &node: func_nodes) {
        func_values.push_back({node->getDuration(), unix_up+node->getRange().from/1000000L});
    }

    int tab_index = tab_widget->addTab(new DisplayValueWidget(func_values, func_name), func_name);
    tab_widget->setCurrentIndex(tab_index);
}

void ProfilerFlameWidget::filterFuncNodes(FuncProfilerTreeNode *node, QString func_name, QVector<FuncProfilerTreeNode *> &func_nodes) {
    if (node->getFuncName() == func_name) {
        func_nodes.push_back(node);
    }

    for (const auto &child: node->getChildren()) {
        filterFuncNodes(child, func_name, func_nodes);
    }
}

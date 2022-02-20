#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tabWidget(nullptr),
#ifdef __linux__
    data_source(new LinuxDataSource)
#else
    data_source(new FreeBSDDataSource)
#endif
{
    ui->setupUi(this);

    tabWidget = new MainTabWidget(data_source, this);
    ui->verticalLayout->addWidget(tabWidget);

    ACTION("CPU Distribution", CPUDistributionWidget);
    ACTION("Receive timestamps", RecvTimestampWidget);
    ACTION("Send timestamps", SendTimestampWidget);
    TAB_ACTION("Sockets list", SocketsListWidget);
    TAB_ACTION("Protocols stats", ProtocolsStatsWidget);
    ACTION("Receive bandwidth", RecvBandwidthWidget);
    ACTION("Send bandwidth", SendBandwidthWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
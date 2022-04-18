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
    TAB_ACTION("Drops info", DropsInfoWidget);
    TAB_ACTION("Sockets list", SocketsListWidget);
    TAB_ACTION("Protocols stats", ProtocolsStatsWidget);


    auto bandwidth_menu = ui->menu->addMenu(tr("Bandwidth"));
    MENU_ACTION(bandwidth_menu, "Receive bandwidth", RecvBandwidthWidget);
    MENU_ACTION(bandwidth_menu, "Send bandwidth", SendBandwidthWidget);
    MENU_ACTION(bandwidth_menu, "Linux Pkt-Gen", LinuxPktgenWidget);

    bandwidth_menu->addAction(tr("Iperf3 Visualize"), Iperf3::openFile(tabWidget));
    bandwidth_menu->addAction(tr("Netmap Pktgen Visualize"), NetmapPktGen::openFile(tabWidget));

    ACTION("Profiler Flame Graph", ProfilerFlameWidget);


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
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define ACTION(name, widget) ui->menu->addAction(tr(name), this, [this]() {this->tabWidget->addTab(new widget(this->data_source), tr(name));})

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    tabWidget(new QTabWidget),
#ifdef __linux__
    data_source(new LinuxDataSource)
#else
    data_source(new FreeBSDDataSource)
#endif
{
    ui->setupUi(this);

    tabWidget->setTabsClosable(true);
    QObject::connect(tabWidget->tabBar(), &QTabBar::tabCloseRequested, tabWidget, &QTabWidget::removeTab);
    ui->verticalLayout->addWidget(tabWidget);

    ACTION("CPU Distribution", CPUDistributionWidget);
    ACTION("Receive timestamps", RecvTimestampWidget);
    ACTION("Send timestamps", SendTimestampWidget);
    ACTION("Sockets list", SocketsListWidget);
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
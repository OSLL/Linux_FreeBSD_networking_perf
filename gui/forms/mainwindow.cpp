#include "mainwindow.h"
#include "ui_mainwindow.h"

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

    addTab<CPUDistributionWidget>("CPU Distribution");
    addTab<RecvTimestampWidget>("Receive timestamps");
    addTab<SendTimestampWidget>("Send timestamps");
    addTab<SocketsListWidget>("Sockets list");
    addTab<ProtocolsStatsWidget>("Protocols stats");
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

template<class T>
void MainWindow::addTab(const char *name) {
    ui->menu->addAction(tr(name), this, [this, name]() {
        int index = this->tabWidget->addTab(new T(this->data_source), tr(name));
        this->tabWidget->setCurrentIndex(index);
    });
}

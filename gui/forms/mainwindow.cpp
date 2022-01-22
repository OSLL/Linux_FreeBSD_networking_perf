#include "cpudistributionwidget.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAction>
#include <QDebug>

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

    ui->menu->addAction(tr("CPU Distribution"), this, [this]() {this->tabWidget->addTab(new CPUDistributionWidget(this->data_source), tr("CPU Distribution"));});
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
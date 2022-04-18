#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QDebug>

#include "cpudistributionwidget.h"
#include "recvtimestampwidget.h"
#include "sendtimestampwidget.h"
#include "socketslistwidget.h"
#include "protocolsstatswidget.h"
#include "recvbandwidthwidget.h"
#include "sendbandwidthwidget.h"
#include "profilerflamewidget.h"
#include "dropsinfowidget.h"

#include "../../types/MainTabWidget.h"
#include "../../datasources/iperf3/iperf3.h"
#include "../../datasources/netmap/netmap.h"

#ifdef __linux__
#include "../../datasources/linux/LinuxDataSource.h"
#else
#include "../../datasources/freebsd/FreeBSDDataSource.h"
#endif

#define ACTION(name, widget) ui->menu->addAction(tr(name), this, tabWidget->getAddFunction<widget>(name))
#define MENU_ACTION(menu, name, widget) menu->addAction(tr(name), this, tabWidget->getAddFunction<widget>(name))
#define TAB_ACTION(name, widget) ui->menu->addAction(tr(name), this, tabWidget->getAddFunctionWithTab<widget>(name))

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::MainWindow *ui;
    MainTabWidget *tabWidget;
    BaseDataSource *data_source;
};

#endif // MAINWINDOW_H

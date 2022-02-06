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

#ifdef __linux__
#include "../../datasources/linux/LinuxDataSource.h"
#include "../../types/MainTabWidget.h"
#else
#include "../../datasources/freebsd/FreeBSDDataSource.h"
#endif

#define ACTION(name, widget) ui->menu->addAction(tr(name), this, tabWidget->getAddFunction<widget>(name))
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

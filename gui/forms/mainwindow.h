#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAction>
#include <QDebug>

#include "cpudistributionwidget.h"
#include "recvtimestampwidget.h"
#include "sendtimestampwidget.h"

#ifdef __linux__
#include "../../datasources/linux/LinuxDataSource.h"
#else
#include "../../datasources/freebsd/FreeBSDDataSource.h"
#endif

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
    QTabWidget *tabWidget;
    BaseDataSource *data_source;
};

#endif // MAINWINDOW_H

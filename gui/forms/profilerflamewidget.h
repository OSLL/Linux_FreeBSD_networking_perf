#ifndef PROFILERFLAMEWIDGET_H
#define PROFILERFLAMEWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QTimer>
#include <QComboBox>
#include <QLineEdit>
#include <QMessageBox>

#include "../../datasources/BaseDataSource.h"
#include "../../types/FlameGraph.h"
#include "../../utils/default_args.h"
#include "../models/ProfilerProtocolsModel.h"
#include "../../types/MainTabWidget.h"
#include "displayvaluewidget.h"

namespace Ui {
class ProfilerFlameWidget;
}

class ProfilerFlameWidget : public QWidget
{
    Q_OBJECT

public:
    ProfilerFlameWidget(BaseDataSource *ds, MainTabWidget *_tab_widget, QWidget *parent = nullptr);
    ~ProfilerFlameWidget();

protected:
    void changeEvent(QEvent *e);
    void filterFuncNodes(FuncProfilerTreeNode *node, QString func_name, QVector<FuncProfilerTreeNode *> &func_nodes);

    QProgressBar *progress_bar;
    QTimer timer;
    BaseDataSource *data_source;
    FlameGraph *flame_graph;
    ProfilerData profiler_data;
    std::unique_ptr<BaseProfilerCollector> collector;
    ProfilerProtocolsModel *protocols_model;
    MainTabWidget *tab_widget;
private:
    Ui::ProfilerFlameWidget *ui;

protected slots:
    void onTimer();
    void CPUChanged(const QString& s_cpu);
    void PIDChanged(const QString& s_pid);
    void onStartClicked();
    void onProtocolClicked(int index);
    void onNodeClick(QMouseEvent *event, const FuncProfilerTreeNode *node);
};

#endif // PROFILERFLAMEWIDGET_H

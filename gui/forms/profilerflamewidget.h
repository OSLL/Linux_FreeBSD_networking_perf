#ifndef PROFILERFLAMEWIDGET_H
#define PROFILERFLAMEWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QTimer>
#include "../../datasources/BaseDataSource.h"
#include "../../types/FlameGraph.h"

namespace Ui {
class ProfilerFlameWidget;
}

class ProfilerFlameWidget : public QWidget
{
    Q_OBJECT

public:
    ProfilerFlameWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~ProfilerFlameWidget();

protected:
    void changeEvent(QEvent *e);

    QProgressBar *progress_bar;
    QTimer timer;
    BaseDataSource *data_source;
    ProfilerParser parser;
    std::unique_ptr<BaseProfilerCollector> collector;
private:
    Ui::ProfilerFlameWidget *ui;

protected slots:
    void onTimer();
};

#endif // PROFILERFLAMEWIDGET_H

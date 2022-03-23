#ifndef PROFILERFLAMEWIDGET_H
#define PROFILERFLAMEWIDGET_H

#include <QWidget>
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

private:
    Ui::ProfilerFlameWidget *ui;
};

#endif // PROFILERFLAMEWIDGET_H

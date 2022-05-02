#ifndef DISPLAYVALUEWIDGET_H
#define DISPLAYVALUEWIDGET_H

#include <QWidget>

#include "../../types/AdvancedChart.h"
#include "../../types/series/TimeSeries.h"
#include "../../types/series/CounterSeries.h"

namespace Ui {
class DisplayValueWidget;
}

class DisplayValueWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayValueWidget(
            QVector<std::pair<quint64, quint64>> data, QString series_name, bool need_average, QWidget *parent = nullptr);
    ~DisplayValueWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DisplayValueWidget *ui;
};

#endif // DISPLAYVALUEWIDGET_H

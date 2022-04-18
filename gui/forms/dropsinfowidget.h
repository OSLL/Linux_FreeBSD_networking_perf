#ifndef DROPSINFOWIDGET_H
#define DROPSINFOWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>

#include "../models/DropsInfoModel.h"
#include "../../datasources/BaseDataSource.h"

namespace Ui {
class DropsInfoWidget;
}

class DropsInfoWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DropsInfoWidget(BaseDataSource *_ds, QWidget *parent = nullptr);
    ~DropsInfoWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::DropsInfoWidget *ui;
    BaseDataSource *data_source;
    QTableView *table_view;
};

#endif // DROPSINFOWIDGET_H

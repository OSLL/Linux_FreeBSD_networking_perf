#ifndef DROPSINFOWIDGET_H
#define DROPSINFOWIDGET_H

#include <QWidget>
#include <QTimer>
#include <QTableView>
#include <QHeaderView>

#include "../models/DropsInfoModel.h"
#include "../models/TableContextMenu.h"
#include "../../datasources/BaseDataSource.h"
#include "../../types/MainTabWidget.h"
#include "trackvaluewidget.h"

namespace Ui {
class DropsInfoWidget;
}

class DropsInfoWidget : public QWidget
{
    Q_OBJECT

public:
    DropsInfoWidget(BaseDataSource *_ds, MainTabWidget *_tab_widget, QWidget *parent = nullptr);
    ~DropsInfoWidget();

protected:
    void changeEvent(QEvent *e);
    void setSpans();

private:
    Ui::DropsInfoWidget *ui;
    BaseDataSource *data_source;
    MainTabWidget *tab_widget;
    QTableView *table_view;
    DropsInfoModel *drops_info_model;
    TableContextMenu menu;
    QTimer timer;

protected slots:
    void onTimeout();
    void onTrackInTab();
};

#endif // DROPSINFOWIDGET_H

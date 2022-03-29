#ifndef SOCKETSLISTWIDGET_H
#define SOCKETSLISTWIDGET_H

#include <QWidget>
#include <QHeaderView>
#include <QTableView>
#include <QTimer>

#include "../../datasources/BaseDataSource.h"
#include "../tables/SocketsListModel.h"
#include "../tables/TableContextMenu.h"
#include "../../utils/default_args.h"
#include "../../types/MainTabWidget.h"

namespace Ui {
class SocketsListWidget;
}

//TODO: Поиск, фильтрация и сортировка в таблице
//TODO: Не показывать пкм меню при клике на первые четыре клетки
class SocketsListWidget : public QWidget
{
    Q_OBJECT

public:
    SocketsListWidget(BaseDataSource *ds, MainTabWidget *_tab_widget, QWidget *parent = nullptr);
    ~SocketsListWidget();

protected:
    void changeEvent(QEvent *e);
    void updateSocketsList();

private:
    Ui::SocketsListWidget *ui;
    BaseDataSource *data_source;
    QTableView *table_view;
    SocketsListModel *model;
    QString protocol;
    QTimer *update_timer;
    MainTabWidget *tab_widget;
    TableContextMenu menu;

protected slots:
    void onProtocolChanged(const QString &_protocol);
    void onTimeout();
    void onTrackInTab();
};

#endif // SOCKETSLISTWIDGET_H

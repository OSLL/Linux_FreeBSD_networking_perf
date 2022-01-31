#ifndef SOCKETSLISTWIDGET_H
#define SOCKETSLISTWIDGET_H

#include <QWidget>
#include <QHeaderView>
#include <QTableView>
#include <QTimer>
#include "../../datasources/BaseDataSource.h"
#include "../tables/SocketsListModel.h"
#include "../tables/SocketsListModel.h"
#include "../../utils/default_args.h"

namespace Ui {
class SocketsListWidget;
}

//TODO: Подстветка подозрительных строк
//TODO: Поиск, фильтрация и сортировка в таблице
//TODO: Построение графиков для выбранных значений (пкм -> построить график в новом окне)
class SocketsListWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SocketsListWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~SocketsListWidget();

protected:
    void changeEvent(QEvent *e);
    void updateSocketsList();

private:
    Ui::SocketsListWidget *ui;
    BaseDataSource *data_source;
    QTableView *table;
    SocketsListModel *model;
    QString protocol;
    QTimer *update_timer;

protected slots:
    void onProtocolChanged(const QString &_protocol);
    void onTimeout();
};

#endif // SOCKETSLISTWIDGET_H

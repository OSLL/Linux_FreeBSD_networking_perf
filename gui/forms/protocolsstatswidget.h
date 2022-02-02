#ifndef PROTOCOLSTATSWIDGET_H
#define PROTOCOLSTATSWIDGET_H

#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <QMessageBox>
#include <QTimer>
#include <QMenu>

#include "../../datasources/BaseDataSource.h"
#include "../tables/ProtocolStatsModel.h"
#include "../../utils/default_args.h"
#include "../../types/MainTabWidget.h"

namespace Ui {
class ProtocolsStatsWidget;
}

class ProtocolsStatsWidget : public QWidget
{
    Q_OBJECT

public:
    ProtocolsStatsWidget(BaseDataSource *ds, MainTabWidget *_tab_widget, QWidget *parent = nullptr);
    ~ProtocolsStatsWidget();

protected:
    void changeEvent(QEvent *e);
    void updateStats();

private:
    Ui::ProtocolsStatsWidget *ui;
    BaseDataSource *data_source;
    QTableView *table_view;
    MainTabWidget *tab_widget;
    ProtocolStatsModel *stats_model;
    std::unique_ptr<QTimer> update_timer;

    QString protocol;

protected slots:
    void onProtocolChanged(const QString &_protocol);
    void onTimeout();
    void onTrackInTab(bool _);
};

#endif // PROTOCOLSTATSWIDGET_H

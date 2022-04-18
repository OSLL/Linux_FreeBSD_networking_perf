//
// Created by shenk on 02.02.2022.
//

#ifndef LFNP_TABLECONTEXTMENU_H
#define LFNP_TABLECONTEXTMENU_H

#include <QMenu>
#include <QTableView>

class TableContextMenu: public QMenu {

protected:

    QTableView *table_view;
    QModelIndex index;
    QSet<int> column_filter;

public:

    TableContextMenu(QTableView *_table_view): table_view(_table_view) {

        table_view->setContextMenuPolicy(Qt::CustomContextMenu);
        QObject::connect(table_view, &QTableView::customContextMenuRequested, this, &TableContextMenu::onMenuRequested);

    }

    void addColumnToFilter(int column) {
        column_filter.insert(column);
    }

    QModelIndex getIndex() { return index; }

private:

    void onMenuRequested(const QPoint &pos) {
        index = table_view->indexAt(pos);
        int column = index.column();

        if (!column_filter.contains(column)) {
            this->popup(table_view->viewport()->mapToGlobal(pos));
        }
    }

};

#endif //LFNP_TABLECONTEXTMENU_H

//
// Created by shenk on 02.02.2022.
//

#ifndef LFNP_MAINTABWIDGET_H
#define LFNP_MAINTABWIDGET_H

#include <QTabWidget>
#include <QTabBar>
#include "../datasources/BaseDataSource.h"

class MainTabWidget: public QTabWidget {

protected:

    BaseDataSource *data_source;
    QWidget *parent;

public:

    MainTabWidget(BaseDataSource *ds, QWidget *_parent): data_source(ds), parent(_parent) {
        setTabsClosable(true);
        QObject::connect(tabBar(), &QTabBar::tabCloseRequested, this, &QTabWidget::removeTab);
    }

    template<class T>
    std::function<void()> getAddFunction(const char *name) {
        return [this, name]() {
            int index = addTab(new T(this->data_source, parent), tr(name));
            this->setCurrentIndex(index);
        };
    }

    template<class T>
    std::function<void()> getAddFunctionWithTab(const char *name) {
        return [this, name]() {
            int index = addTab(new T(this->data_source, this, parent), tr(name));
            this->setCurrentIndex(index);
        };
    }

};

#endif //LFNP_MAINTABWIDGET_H

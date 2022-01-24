#ifndef RECVTIMESTAMPWIDGET_H
#define RECVTIMESTAMPWIDGET_H

#include <QWidget>
#include "../../datasources/BaseDataSource.h"

namespace Ui {
class RecvTimestampWidget;
}

class RecvTimestampWidget : public QWidget
{
    Q_OBJECT

public:
    RecvTimestampWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~RecvTimestampWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RecvTimestampWidget *ui;
    BaseDataSource *data_source;
};

#endif // RECVTIMESTAMPWIDGET_H

#ifndef SENDTIMESTAMPWIDGET_H
#define SENDTIMESTAMPWIDGET_H

#include <QWidget>

#include "../../utils/default_args.h"
#include "../../types/enums/MeasureType.h"
#include "../../utils/sockets.h"
#include "../../datasources/BaseDataSource.h"
#include "../threads/TimestampsSenderThread.h"

namespace Ui {
class SendTimestampWidget;
}

using namespace std::placeholders;

class SendTimestampWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SendTimestampWidget(BaseDataSource *ds, QWidget *parent = nullptr);
    ~SendTimestampWidget();

protected:
    void changeEvent(QEvent *e);

protected slots:
    void onStartClicked();

private:

    Ui::SendTimestampWidget *ui;
    TimestampsSenderThread *sender;
    BaseDataSource *data_source;
};

#endif // SENDTIMESTAMPWIDGET_H

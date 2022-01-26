#ifndef SENDTIMESTAMPWIDGET_H
#define SENDTIMESTAMPWIDGET_H

#include <QWidget>

#include "../../utils/default_args.h"
#include "../../types/enums/MeasureType.h"
#include "../../utils/sockets.h"
#include "../../datasources/BaseDataSource.h"

namespace Ui {
class SendTimestampWidget;
}

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
};

#endif // SENDTIMESTAMPWIDGET_H

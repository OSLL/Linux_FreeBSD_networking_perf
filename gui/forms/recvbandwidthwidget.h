#ifndef RECVBANDWIDTHWIDGET_H
#define RECVBANDWIDTHWIDGET_H

#include <QWidget>

namespace Ui {
class RecvBandwidthWidget;
}

class RecvBandwidthWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RecvBandwidthWidget(QWidget *parent = nullptr);
    ~RecvBandwidthWidget();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::RecvBandwidthWidget *ui;
};

#endif // RECVBANDWIDTHWIDGET_H

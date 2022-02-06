#ifndef STARTSTOPWIDGET_H
#define STARTSTOPWIDGET_H

#include <QWidget>

namespace Ui {
class StartStopWidget;
}

class StartStopWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartStopWidget(QWidget *parent = nullptr);
    ~StartStopWidget();

    void setStartState();
    void setStopState();

protected:
    void changeEvent(QEvent *e);

private:
    Ui::StartStopWidget *ui;

protected slots:
    void onStartButton();
    void onStopButton();

    signals:
    void start();
    void stop();
};

#endif // STARTSTOPWIDGET_H

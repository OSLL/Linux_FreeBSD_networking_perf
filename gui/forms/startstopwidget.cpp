#include "startstopwidget.h"
#include "ui_startstopwidget.h"

StartStopWidget::StartStopWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartStopWidget)
{
    ui->setupUi(this);
    this->layout()->setAlignment(Qt::AlignmentFlag::AlignTop);

    setStartState();
    QObject::connect(ui->startButton, &QPushButton::clicked, this, &StartStopWidget::onStartButton);
    QObject::connect(ui->stopButton, &QPushButton::clicked, this, &StartStopWidget::onStopButton);
}

StartStopWidget::~StartStopWidget()
{
    delete ui;
}

void StartStopWidget::changeEvent(QEvent *e)
{
    QWidget::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}

void StartStopWidget::onStartButton() {

    setStopState();
    emit start();

}

void StartStopWidget::onStopButton() {

    setStartState();
    emit stop();

}

void StartStopWidget::setStartState() {void setStartState();

    ui->startButton->setDisabled(false);
    ui->stopButton->setDisabled(true);

}

void StartStopWidget::setStopState() {

    ui->startButton->setDisabled(true);
    ui->stopButton->setDisabled(false);

}

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QTime>
#include <QTextBrowser>

Stopwatch::Stopwatch(QObject *parent) : QObject(parent), running(false) {
    timer = new QTimer(this);
    elapsedTime = new QTime(0, 0);
    connect(timer, &QTimer::timeout, this, &Stopwatch::updateTime);
}

void Stopwatch::start() {
    if (!running) {
        timer->start(100);  // обновление каждую 0.1 секунду
        running = true;
    }
}

void Stopwatch::stop() {
    if (running) {
        timer->stop();
        running = false;
        pausedTime = elapsedTime->second() + elapsedTime->minute() * 60 + elapsedTime->hour() * 3600; // Сохраняем время на момент остановки
    }
}

void Stopwatch::reset() {
    timer->stop();

    elapsedTime->setHMS(0, 0, 0);
    pausedTime = 0.0;
    running = false;
    emit timeUpdated(0);
}

void Stopwatch::resume() {
    if (!running) {
        *elapsedTime = QTime(0, 0).addSecs(pausedTime);  // Возвращаем время на момент остановки
        start();  // Продолжаем отсчет
    }
}

void Stopwatch::updateTime() {
    *elapsedTime = elapsedTime->addMSecs(100);
    emit timeUpdated(elapsedTime->second() + elapsedTime->minute() * 60 + elapsedTime->hour() * 3600);
}

double Stopwatch::getTime() const {
    return elapsedTime->second() + elapsedTime->minute() * 60 + elapsedTime->hour() * 3600;
}

bool Stopwatch::isRunning() const {
    return running;  // Проверяем, работает ли секундомер
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    stopwatch(new Stopwatch(this)),
    lapCounter(0)
{
    ui->setupUi(this);
    connect(ui->pb_startStopButton, &QPushButton::clicked, this, &MainWindow::on_startStopButton_clicked);
    connect(ui->pb_clearButton, &QPushButton::clicked, this, &MainWindow::on_clearButton_clicked);
    connect(ui->pb_lapButton, &QPushButton::clicked, this, &MainWindow::on_lapButton_clicked);
    connect(stopwatch, &Stopwatch::timeUpdated, this, &MainWindow::updateDisplay);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_startStopButton_clicked() {
    QPushButton *startStopButton = ui->pb_startStopButton;

    if (stopwatch->getTime() == 0 && !stopwatch->isRunning()) {
        stopwatch->start();  // Если секундомер не работает, стартуем
        startStopButton->setText("Стоп");
    } else {
        if (stopwatch->isRunning()) {
            stopwatch->stop();  // Если секундомер работает, остановим
            startStopButton->setText("Старт");
        } else {
            stopwatch->resume();  // Если секундомер остановлен, продолжим отсчет с последнего времени
            startStopButton->setText("Стоп");
        }
    }
}

void MainWindow::on_clearButton_clicked() {
    lapCounter = 0;
    ui->tB_lapBrowser->clear();
    ui->pb_startStopButton->setText("Старт");
    ui->lb_timeLabel->setText("0.0");
    stopwatch->reset();
}

void MainWindow::on_lapButton_clicked() {
    if (stopwatch->getTime() > 0) {
        lapCounter++;
        QString lapTime = QString("Круг %1, время: %2 сек").arg(lapCounter).arg(stopwatch->getTime());
        ui->tB_lapBrowser->append(lapTime);
    }
}

void MainWindow::updateDisplay(double time) {
    ui->lb_timeLabel->setText(QString::number(time, 'f', 1));
}

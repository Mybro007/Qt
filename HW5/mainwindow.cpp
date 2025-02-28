#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QTime>
#include <QTextBrowser>

Stopwatch::Stopwatch(QObject *parent) : QObject(parent), running(false), elapsedMilliseconds(0), lastLapTime(0) {
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Stopwatch::updateTime);
}

void Stopwatch::start() {
    if (!running) {
        timer->start(100);  // Обновление каждую 0.1 секунду
        running = true;
    }
}

void Stopwatch::stop() {
    if (running) {
        timer->stop();
        running = false;
    }
}

void Stopwatch::reset() {
    elapsedMilliseconds = 0;  // Сбрасываем время
    lastLapTime = 0;  // Сбрасываем время последнего круга
    emit timeUpdated(elapsedMilliseconds / 1000.0);  // Отправляем сброшенное время
}

void Stopwatch::resume() {
    if (!running) {
        timer->start(100);  // Продолжаем отсчет
        running = true;
    }
}

void Stopwatch::updateTime() {
    elapsedMilliseconds += 100;  // Добавляем 100 миллисекунд
    emit timeUpdated(elapsedMilliseconds / 1000.0);  // Отправляем обновленное время в секундах
}

double Stopwatch::getTime() const {
    return elapsedMilliseconds / 1000.0;  // Возвращаем время в секундах с точностью до 1 знака
}

bool Stopwatch::isRunning() const {
    return running;  // Проверяем, работает ли секундомер
}

double Stopwatch::getLapTime() {
    return (elapsedMilliseconds - lastLapTime) / 1000.0;  // Возвращаем время круга в секундах с точностью до миллисекунд
}

void Stopwatch::lastLapTimeUpdate(){
    lastLapTime = elapsedMilliseconds;
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
        ui->pb_lapButton->setEnabled(true);  // Разблокируем кнопку круга
    } else {
        if (stopwatch->isRunning()) {
            stopwatch->stop();  // Если секундомер работает, остановим
            startStopButton->setText("Старт");
            ui->pb_lapButton->setEnabled(false);  // Блокируем кнопку круга
        } else {
            stopwatch->resume();  // Если секундомер остановлен, продолжим отсчет с последнего времени
            startStopButton->setText("Стоп");
            ui->pb_lapButton->setEnabled(true);  // Разблокируем кнопку круга
        }
    }
}

void MainWindow::on_clearButton_clicked() {
    lapCounter = 0;
    ui->tB_lapBrowser->clear();
    ui->lb_timeLabel->setText("0.0");  // Обновляем отображаемое время
    stopwatch->reset();
}

void MainWindow::on_lapButton_clicked() {
    if (stopwatch->getTime() > 0) {
        double lapTime = stopwatch->getLapTime();  // Получаем время круга с точностью до миллисекунд
        lapCounter++;

        // Форматируем время круга с точностью до 3 знаков после запятой
        QString lapTimeStr = QString("Круг %1, время: %2 сек").arg(lapCounter).arg(lapTime, 0, 'f', 1);

        // Добавляем в журнал кругов
        ui->tB_lapBrowser->append(lapTimeStr);

        // Обновляем время последнего круга
        stopwatch->lastLapTimeUpdate();
    }
}

void MainWindow::updateDisplay(double time) {
    ui->lb_timeLabel->setText(QString::number(time, 'f', 1));
}

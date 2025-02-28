#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QtConcurrent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->sb_initNum->setValue(200000);
    ui->rb_synchOff->setChecked(true);

    race1 = new Controller(&m);
    race2 = new Controller(&m);

    concurRace1 = new ExampleRace(&m);
    concurRace2 = new ExampleRace(&m);

    // Сигналы по завершению работы потоков
    connect(race1, &Controller::sig_WorkFinish, this, &MainWindow::onWorkFinish);
    connect(race2, &Controller::sig_WorkFinish, this, &MainWindow::onWorkFinish);
    connect(concurRace1, &ExampleRace::sig_Finish, this, &MainWindow::onWorkFinish);
    connect(concurRace2, &ExampleRace::sig_Finish, this, &MainWindow::onWorkFinish);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete race1;
    delete race2;
    delete concurRace1;
    delete concurRace2;
}

// Метод запускает два потока
void MainWindow::StartRace(void){
    if(ui->rb_qtConcur->isChecked()){

        // Убедимся, что мы начинаем с правильного состояния
        countFinish = 0;
        number = 0;

        // Создаем QFutureWatcher для отслеживания завершения обоих потоков
        QFutureWatcher<void> watcher1;
        QFutureWatcher<void> watcher2;

        // Создаем два потока, которые будут инкрементировать значение
        QFuture<void> future1 = QtConcurrent::run([=]() {
            concurRace1->DoWork(&number, ui->rb_mutexOn->isChecked(), ui->sb_initNum->value());
        });

        QFuture<void> future2 = QtConcurrent::run([=]() {
            concurRace2->DoWork(&number, ui->rb_mutexOn->isChecked(), ui->sb_initNum->value());
        });

        // Подключаем сигнал завершения для обоих потоков
        connect(&watcher1, &QFutureWatcher<void>::finished, this, [&]() {
            countFinish++;
            if (countFinish == 2) {
                ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                                     QString::number(ui->sb_initNum->value() * 2));
                ui->pb_start->setEnabled(true);
            }
        });

        connect(&watcher2, &QFutureWatcher<void>::finished, this, [&]() {
            countFinish++;
            if (countFinish == 2) {
                ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                                     QString::number(ui->sb_initNum->value() * 2));
                ui->pb_start->setEnabled(true);
            }
        });

        // Привязываем QFutureWatcher к нашим потокам
        watcher1.setFuture(future1);
        watcher2.setFuture(future2);

    }
    else{
        emit race1->operate(&number, ui->rb_mutexOn->isChecked(), ui->sb_initNum->value());
        emit race2->operate(&number, ui->rb_mutexOn->isChecked(), ui->sb_initNum->value());
    }
}

// Обработка кнопки "Старт"
void MainWindow::on_pb_start_clicked()
{
    ui->pb_start->setEnabled(false);
    countFinish = 0;
    number = 0;
    StartRace();
}

void MainWindow::onWorkFinish()
{
    if (countFinish == 0) {
        countFinish++;
    }
    else {
        ui->te_debug->append("Искомое число равно: " + QString::number(number) + ", а должно быть " +
                             QString::number(ui->sb_initNum->value() * 2));
        ui->pb_start->setEnabled(true);
    }
}

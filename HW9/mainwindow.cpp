#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // Инициализация рабочего объекта для UDP
    udpWorker = new UDPworker(this);
    udpWorker->InitSocket();

    // Подключение сигналов для обработки времени и принятой датаграммы
    connect(udpWorker, &UDPworker::sig_sendTimeToGUI, this, &MainWindow::DisplayTime);
    connect(udpWorker, &UDPworker::sig_sendReceivedData, this, &MainWindow::DisplayReceivedData);

    // Подключение кнопки для отправки датаграммы
    connect(ui->pb_sendDatagram, &QPushButton::clicked, this, &MainWindow::onSendDatagramClicked);

    // Таймер для отправки датаграмм с интервалом
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [&]{
        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray dataToSend;
        QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

        outStr << dateTime;

        udpWorker->SendDatagram(dataToSend);
        timer->start(TIMER_DELAY);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    timer->start(TIMER_DELAY);
}

void MainWindow::DisplayTime(QDateTime data)
{
    counterPck++;
    if (counterPck % 20 == 0) {
        ui->te_result->clear();
    }

    ui->te_result->append("Текущее время: " + data.toString() + ". "
                                                                            "Принято пакетов " + QString::number(counterPck));
}

void MainWindow::on_pb_stop_clicked()
{
    timer->stop();
}

// Новый слот для обработки нажатия кнопки "Отправить датаграмму"
void MainWindow::onSendDatagramClicked()
{
    // Получаем текст из поля ввода
    QString text = ui->te_inputText->toPlainText();

    // Проверка на пустой ввод
    if (text.isEmpty()) {
        ui->te_result->append("Ошибка: Текст не введен!");
        return;
    }

    // Преобразуем текст в QByteArray
    QByteArray dataToSend = text.toUtf8();

    // Отправляем данные
    udpWorker->SendDatagram(dataToSend);

    // Отображаем отправленный текст в консоли
    ui->te_result->append("Отправлена датаграмма: " + text);
}

// Новый слот для отображения информации о принятом сообщении
void MainWindow::DisplayReceivedData(QString senderAddress, int messageSize)
{
    QString message = QString("Принято сообщение от адреса %1, размер сообщения (байт): %2")
                          .arg(senderAddress).arg(messageSize);
    ui->te_result->append(message);
}

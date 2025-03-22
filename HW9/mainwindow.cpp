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
        // Отправка времени каждую секунду
        QDateTime dateTime = QDateTime::currentDateTime();

        QByteArray timeToSend;
        QDataStream outStr(&timeToSend, QIODevice::WriteOnly);
        outStr << QByteArray("T");  // Маркер времени
        outStr << dateTime;

        // Логирование отправляемых данных
        qDebug() << "Отправка времени:" << dateTime.toString();

        udpWorker->SendDatagram(timeToSend);
        timer->start(TIMER_DELAY);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pb_start_clicked()
{
    // При нажатии на "Start" начинается отправка времени
    qDebug() << "Запуск таймера для отправки времени";
    timer->start(TIMER_DELAY);
}

void MainWindow::DisplayTime(QDateTime data)
{
    counterPck++;
    if (counterPck % 20 == 0) {
        ui->te_result->clear();
    }

    // Логирование времени
    qDebug() << "Получено время:" << data.toString();

    // Отображаем только время, без информации о размере
    ui->te_result->append("Текущее время: " + data.toString() + ". Принято пакетов " + QString::number(counterPck));
}

void MainWindow::on_pb_stop_clicked()
{
    // Останавливаем таймер
    qDebug() << "Остановка таймера";
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

    // Создаем маркер для передачи
    QByteArray dataToSend;
    QDataStream outStr(&dataToSend, QIODevice::WriteOnly);

    // Отправляем текст
    outStr << QByteArray("S");  // Маркер строки
    outStr << text;

    // Логируем отправленные данные
    qDebug() << "Отправка данных:" << dataToSend;

    // Отправляем данные
    udpWorker->SendDatagram(dataToSend);

    // Отображаем информацию о количестве отправленных байт
    ui->te_result->append(QString("Отправлена датаграмма: %1, размер: %2 байт")
                              .arg(text).arg(dataToSend.size()));
}

// Новый слот для отображения информации о принятом сообщении
void MainWindow::DisplayReceivedData(QString senderAddress, int messageSize)
{
    // Логирование принятого пакета
    qDebug() << "Принято сообщение от адреса" << senderAddress << "размер сообщения" << messageSize << "байт";

    // Выводим только информацию о принятом сообщении с размером
    ui->te_result->append(QString("Принято сообщение от адреса %1, размер сообщения (байт): %2")
                              .arg(senderAddress).arg(messageSize));
}

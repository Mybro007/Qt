#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{
}

void UDPworker::InitSocket()
{
    serviceUdpSocket = new QUdpSocket(this);

    serviceUdpSocket->bind(QHostAddress::LocalHost, PORT);

    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);
}

void UDPworker::ReadDatagram(QNetworkDatagram datagram)
{
    QByteArray data = datagram.data();

    // Логирование размера принятого пакета
    qDebug() << "Принят пакет, размер:" << data.size() << "байт";

    // Создаем поток для десериализации
    QDataStream inStr(&data, QIODevice::ReadOnly);

    // Читаем маркер (тип данных)
    QByteArray dataType;
    inStr >> dataType;

    // Проверяем маркер и интерпретируем данные в зависимости от типа
    if (dataType == QByteArray("T")) {
        // Это маркер времени, читаем QDateTime
        QDateTime receivedTime;
        inStr >> receivedTime;

        if (receivedTime.isValid()) {
            qDebug() << "Принято время:" << receivedTime.toString();
            emit sig_sendTimeToGUI(receivedTime);
        } else {
            qDebug() << "Не удалось интерпретировать как время";
        }
    } else if (dataType == QByteArray("S")) {
        // Это маркер строки, читаем строку
        QString receivedText;
        inStr >> receivedText;

        qDebug() << "Принято текстовое сообщение:" << receivedText;

        // Логируем информацию о принятом сообщении
        QString senderAddress = datagram.senderAddress().toString();
        int messageSize = data.size();
        emit sig_sendReceivedData(senderAddress, messageSize);
    } else {
        // Неверный маркер данных
        qDebug() << "Неизвестный тип данных";
    }
}



void UDPworker::SendDatagram(QByteArray data)
{
    // Логирование отправляемых данных
    qDebug() << "Отправка данных, размер:" << data.size() << "байт";

    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, PORT);
}

void UDPworker::readPendingDatagrams()
{
    while (serviceUdpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}

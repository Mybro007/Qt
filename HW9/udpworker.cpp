#include "udpworker.h"

UDPworker::UDPworker(QObject *parent) : QObject(parent)
{
}

void UDPworker::InitSocket()
{
    serviceUdpSocket = new QUdpSocket(this);

    serviceUdpSocket->bind(QHostAddress::LocalHost, BIND_PORT);

    connect(serviceUdpSocket, &QUdpSocket::readyRead, this, &UDPworker::readPendingDatagrams);
}

void UDPworker::ReadDatagram(QNetworkDatagram datagram)
{
    QByteArray data = datagram.data();

    // Логирование размера принятого пакета
    qDebug() << "Принят пакет, размер:" << data.size() << "байт";

    // Попытка десериализации данных в QDateTime
    QDataStream inStr(&data, QIODevice::ReadOnly);

    // Попробуем прочитать QDateTime из потока
    QDateTime receivedTime;
    inStr >> receivedTime;

    if (receivedTime.isValid()) {
        // Если мы успешно получили QDateTime, значит это переданное время
        qDebug() << "Принято время:" << receivedTime.toString();
        emit sig_sendTimeToGUI(receivedTime);
    } else {
        // Если это не время, передаем информацию о размере и адресе отправителя
        QString senderAddress = datagram.senderAddress().toString();
        int messageSize = data.size();

        // Логирование информации о принятом сообщении
        qDebug() << "Принято сообщение от адреса" << senderAddress << "размер сообщения" << messageSize << "байт";
        emit sig_sendReceivedData(senderAddress, messageSize);
    }
}

void UDPworker::SendDatagram(QByteArray data)
{
    // Логирование отправляемых данных
    qDebug() << "Отправка данных, размер:" << data.size() << "байт";

    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT);
}

void UDPworker::readPendingDatagrams()
{
    while (serviceUdpSocket->hasPendingDatagrams()) {
        QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}

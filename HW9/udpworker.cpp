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

    // Отправляем принятые данные в GUI
    QString receivedMessage = QString::fromUtf8(data);
    QString senderAddress = datagram.senderAddress().toString();
    int messageSize = data.size();

    emit sig_sendTimeToGUI(QDateTime::currentDateTime());

    // Отображаем информацию о принятом сообщении
    emit sig_sendReceivedData(senderAddress, messageSize);
}

void UDPworker::SendDatagram(QByteArray data)
{
    serviceUdpSocket->writeDatagram(data, QHostAddress::LocalHost, BIND_PORT);
}

void UDPworker::readPendingDatagrams()
{
    while(serviceUdpSocket->hasPendingDatagrams()){
        QNetworkDatagram datagram = serviceUdpSocket->receiveDatagram();
        ReadDatagram(datagram);
    }
}

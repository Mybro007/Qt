#ifndef UDPWORKER_H
#define UDPWORKER_H

#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDateTime>

#define PORT 12345

class UDPworker : public QObject
{
    Q_OBJECT
public:
    explicit UDPworker(QObject *parent = nullptr);
    void InitSocket();
    void ReadDatagram(QNetworkDatagram datagram);
    void SendDatagram(QByteArray data);

private slots:
    void readPendingDatagrams();

private:
    QUdpSocket* serviceUdpSocket;

signals:
    void sig_sendTimeToGUI(QDateTime data);
    void sig_sendReceivedData(QString senderAddress, int messageSize);  // Новый сигнал для передачи данных

};

#endif // UDPWORKER_H

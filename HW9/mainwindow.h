#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTextEdit>
#include "UdpWorker.h"

#define TIMER_DELAY 1000

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pb_start_clicked();                   // Слот для старта таймера
    void DisplayTime(QDateTime data);             // Слот для отображения времени
    void on_pb_stop_clicked();                    // Слот для остановки таймера
    void onSendDatagramClicked();                 // Слот для отправки датаграммы
    void DisplayReceivedData(QString senderAddress, int messageSize);  // Слот для отображения информации о принятом сообщении

private:
    Ui::MainWindow *ui;                          // Указатель на UI
    QTimer* timer;                               // Таймер для отправки датаграмм
    UDPworker* udpWorker;                        // Объект для работы с UDP
    uint32_t counterPck = 0;                     // Счётчик пакетов
};

#endif // MAINWINDOW_H

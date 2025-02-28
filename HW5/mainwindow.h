#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QTime>
#include <QTextBrowser>

namespace Ui {
class MainWindow;
}

class Stopwatch : public QObject {
    Q_OBJECT
public:
    explicit Stopwatch(QObject *parent = nullptr);
    void start();
    void stop();
    void reset();
    void resume();
    double getTime() const;
    bool isRunning() const;
    double getLapTime();
    void lastLapTimeUpdate();

signals:
    void timeUpdated(double time);

private slots:
    void updateTime();

private:
    QTimer *timer;
    double elapsedMilliseconds;
    bool running;
    double pausedTime;
    double lastLapTime;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_startStopButton_clicked();
    void on_clearButton_clicked();
    void on_lapButton_clicked();
    void updateDisplay(double time);

private:
    Ui::MainWindow *ui;
    Stopwatch *stopwatch;
    int lapCounter;
};

#endif // MAINWINDOW_H

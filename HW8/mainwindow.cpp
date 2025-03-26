#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dbdata.h"
#include <QMessageBox>
#include <QSqlQuery>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , dataDb(new DbData(this))
    , dataBase(new DataBase(this))
{
    ui->setupUi(this);

    // Настройка интерфейса
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);
    ui->tb_result->setColumnCount(2);
    ui->tb_result->setHorizontalHeaderLabels({"Название фильма", "Год выпуска"});

    // Подключение сигналов и слотов
    connect(dataDb, &DbData::sig_sendData, this, &MainWindow::onDatabaseDataReceived);
    connect(dataBase, &DataBase::connectionStatusChanged, this, &MainWindow::onConnectionStatusChanged);
    connect(ui->act_connect, &QAction::triggered, this, &MainWindow::onActConnectTriggered);
    connect(ui->act_addData, &QAction::triggered, this, &MainWindow::onActAddDataTriggered);
    connect(ui->pb_request, &QPushButton::clicked, this, &MainWindow::onPbRequestClicked);
    connect(ui->pb_clear, &QPushButton::clicked, this, &MainWindow::onPbClearClicked);

    // Инициализация базы данных
    dataBase->connectToDatabase();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dataDb;
    delete dataBase;
}

void MainWindow::onActAddDataTriggered()
{
    dataDb->show();
}

void MainWindow::onActConnectTriggered()
{
    if (dataBase->isConnected()) {
        dataBase->disconnectFromDatabase();
    } else {
        dataBase->connectToDatabase();
    }
}

void MainWindow::onPbRequestClicked()
{
    if (!dataBase->isConnected()) {
        QMessageBox::warning(this, "Ошибка", "Нет подключения к базе данных");
        return;
    }

    QString category = ui->cb_category->currentText();
    QString query;

    if (category == "Все") {
        query = "SELECT title, release_year FROM film";
    } else if (category == "Комедия") {
        query = "SELECT f.title, f.release_year FROM film f "
                "JOIN film_category fc ON f.film_id = fc.film_id "
                "JOIN category c ON c.category_id = fc.category_id "
                "WHERE c.name = 'Comedy'";
    } else if (category == "Ужасы") {
        query = "SELECT f.title, f.release_year FROM film f "
                "JOIN film_category fc ON f.film_id = fc.film_id "
                "JOIN category c ON c.category_id = fc.category_id "
                "WHERE c.name = 'Horror'";
    }

    QVector<QVector<QVariant>> results;
    if (dataBase->executeQuery(query, &results)) {
        ui->tb_result->setRowCount(results.size());
        for (int i = 0; i < results.size(); ++i) {
            const auto& row = results[i];
            ui->tb_result->setItem(i, 0, new QTableWidgetItem(row[0].toString()));
            ui->tb_result->setItem(i, 1, new QTableWidgetItem(row[1].toString()));
        }
    } else {
        QMessageBox::critical(this, "Ошибка запроса", dataBase->lastError().text());
    }
}

void MainWindow::onPbClearClicked()
{
    ui->tb_result->clearContents();
    ui->tb_result->setRowCount(0);
}

void MainWindow::onDatabaseDataReceived(QVector<QString> dbData)
{
    // Здесь можно сохранить данные для подключения
    Q_UNUSED(dbData);
}

void MainWindow::onConnectionStatusChanged(bool connected)
{
    if (connected) {
        ui->lb_statusConnect->setText("Подключено");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->act_connect->setText("Отключиться");
        ui->pb_request->setEnabled(true);
    } else {
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->act_connect->setText("Подключиться");
        ui->pb_request->setEnabled(false);
    }
}

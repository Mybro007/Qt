#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    dataDb = new DbData(this);
    dataBase = new DataBase(this);
    msg = new QMessageBox(this);

    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);

    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });

    connect(dataBase, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_act_addData_triggered()
{
    //Отобразим диалоговое окно. Какой метод нужно использовать?
    dataDb->show();
}

void MainWindow::on_pb_clear_clicked()
{
    ui->tb_result->clearContents(); // Очистить содержимое
    ui->tb_result->setRowCount(0);  // Удалить все строки
}

void MainWindow::on_act_connect_triggered()
{
    /*
     * Обработчик кнопки у нас должен подключаться и отключаться от БД.
     * Можно привязаться к надписи лейбла статуса. Если он равен
     * "Отключено" мы осуществляем подключение, если "Подключено" то
     * отключаемся
    */

    if(ui->lb_statusConnect->text() == "Отключено"){

        ui->lb_statusConnect->setText("Подключение");
        ui->lb_statusConnect->setStyleSheet("color : black");

        // Лямбда-функция для подключения
        auto conn = [&]{
            // Подключаемся с захардкоженными данными
            dataBase->ConnectToDataBase(dataForConnect);
        };
        QtConcurrent::run(conn);  // Выполняем подключение в отдельном потоке

    }
    else{
        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }
}

void MainWindow::on_pb_request_clicked()
{
    ui->tb_result->clearContents();
    ui->tb_result->setRowCount(0);

    if (ui->cb_category->currentText() == "Все") {
        QSqlTableModel* model = new QSqlTableModel(this);
        model->setTable("film");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);

        // Связываем с базой данных через глобальное подключение
        model->setQuery("SELECT title, description FROM film");  // Выполнение запроса

        if (model->rowCount() > 0) {
            for (int i = 0; i < model->rowCount(); ++i) {
                ui->tb_result->insertRow(i);
                ui->tb_result->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Название фильма
                ui->tb_result->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));  // Описание фильма
            }
            ui->tb_result->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
        }
    }
    else if (ui->cb_category->currentText() == "Комедия" || ui->cb_category->currentText() == "Ужасы") {
        QString category = ui->cb_category->currentText();
        QSqlQueryModel* model = new QSqlQueryModel(this);
        QString queryStr = QString(
                               "SELECT f.title, f.description "
                               "FROM film f "
                               "JOIN film_category fc ON f.film_id = fc.film_id "
                               "JOIN category c ON c.category_id = fc.category_id "
                               "WHERE c.name = '%1'").arg(category);

        model->setQuery(queryStr);  // Выполнение запроса

        if (model->rowCount() > 0) {
            for (int i = 0; i < model->rowCount(); ++i) {
                ui->tb_result->insertRow(i);
                ui->tb_result->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));  // Название фильма
                ui->tb_result->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Описание фильма
            }
            ui->tb_result->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
        }
    }
}


void MainWindow::ScreenDataFromDB(QTableWidget* widget, int typeRequest)
{
    widget->clearContents();
    widget->setRowCount(0);

    if (typeRequest == requestAllFilms) {
        // Используем QSqlTableModel, создаём модель и подключаем к базе данных
        QSqlTableModel* model = new QSqlTableModel(this);
        model->setTable("film");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->setQuery("SELECT title, description FROM film");  // Выполнение запроса

        if (model->rowCount() > 0) {
            for (int i = 0; i < model->rowCount(); ++i) {
                widget->insertRow(i);
                widget->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));  // Название фильма
                widget->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Описание фильма
            }
            widget->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
        }
    }
    else if (typeRequest == requestComedy || typeRequest == requestHorrors) {
        // Используем QSqlQueryModel для выполнения запроса
        QSqlQueryModel* model = new QSqlQueryModel(this);
        QString category = (typeRequest == requestComedy) ? "Comedy" : "Horror";
        QString queryStr = QString(
                               "SELECT f.title, f.description "
                               "FROM film f "
                               "JOIN film_category fc ON f.film_id = fc.film_id "
                               "JOIN category c ON c.category_id = fc.category_id "
                               "WHERE c.name = '%1'").arg(category);

        model->setQuery(queryStr);  // Выполнение запроса

        if (model->rowCount() > 0) {
            for (int i = 0; i < model->rowCount(); ++i) {
                widget->insertRow(i);
                widget->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));  // Название фильма
                widget->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Описание фильма
            }
            widget->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
        }
    }
}

void MainWindow::ReceiveStatusConnectionToDB(bool status)
{
    if (status) {
        ui->act_connect->setText("Отключиться");
        ui->lb_statusConnect->setText("Подключено к БД");
        ui->lb_statusConnect->setStyleSheet("color:green");
        ui->pb_request->setEnabled(true);
    }
    else {
        dataBase->DisconnectFromDataBase(DB_NAME);
        msg->setIcon(QMessageBox::Critical);
        msg->setText(dataBase->GetLastError().text());
        ui->lb_statusConnect->setText("Отключено");
        ui->lb_statusConnect->setStyleSheet("color:red");
        msg->exec();
    }
}




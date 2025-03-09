#include "mainwindow.h"
#include "./ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    //Исходное состояние виджетов
    ui->setupUi(this);
    ui->lb_statusConnect->setStyleSheet("color:red");
    ui->pb_request->setEnabled(false);

    /*
     * Выделим память под необходимые объекты. Все они наследники
     * QObject, поэтому воспользуемся иерархией.
    */

    dataDb = new DbData(this);
    dataBase = new DataBase(this);
    msg = new QMessageBox(this);

    //Установим размер вектора данных для подключения к БД
    dataForConnect.resize(NUM_DATA_FOR_CONNECT_TO_DB);

    /*
     * Добавим БД используя стандартный драйвер PSQL и зададим имя.
    */
    dataBase->AddDataBase(POSTGRE_DRIVER, DB_NAME);

    /*
     * Устанавливаем данные для подключениея к БД.
     * Поскольку метод небольшой используем лямбда-функцию.
     */
    connect(dataDb, &DbData::sig_sendData, this, [&](QVector<QString> receivData){
        dataForConnect = receivData;
    });

    /*
     * Соединяем сигнал, который передает ответ от БД с методом, который отображает ответ в ПИ
     */
    connect(dataBase, &DataBase::sig_SendDataFromDB, this, &MainWindow::ScreenDataFromDB);

    /*
     *  Сигнал для подключения к БД
     */
    connect(dataBase, &DataBase::sig_SendStatusConnection, this, &MainWindow::ReceiveStatusConnectionToDB);

}

MainWindow::~MainWindow()
{
    delete ui;
}

/*!
 * @brief Слот отображает форму для ввода данных подключения к БД
 */
void MainWindow::on_act_addData_triggered()
{
    //Отобразим диалоговое окно. Какой метод нужно использовать?
    dataDb->show();
}

/*!
 * @brief Слот выполняет подключение к БД. И отображает ошибки.
 */

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


        auto conn = [&]{dataBase->ConnectToDataBase(dataForConnect);};
        QtConcurrent::run(conn);

    }
    else{
        dataBase->DisconnectFromDataBase(DB_NAME);
        ui->lb_statusConnect->setText("Отключено");
        ui->act_connect->setText("Подключиться");
        ui->lb_statusConnect->setStyleSheet("color:red");
        ui->pb_request->setEnabled(false);
    }

}

/*!
 * \brief Обработчик кнопки "Получить"
 */
void MainWindow::on_pb_request_clicked()
{
    // Очищаем таблицу перед загрузкой новых данных
    ui->tb_result->clearContents();
    ui->tb_result->setRowCount(0);  // Убираем все строки, чтобы не было остатков от предыдущего запроса

    if (ui->cb_category->currentText() == "Все") {
        // Запрос для получения всех фильмов через QSqlTableModel
        QSqlTableModel* model = new QSqlTableModel(this);
        model->setTable("film");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();

        if (model->rowCount() > 0) {
            // Заполняем таблицу данными
            for (int i = 0; i < model->rowCount(); ++i) {
                ui->tb_result->insertRow(i);  // Добавляем строку
                ui->tb_result->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Название фильма
                ui->tb_result->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));  // Описание фильма
            }

            // Устанавливаем заголовки столбцов
            ui->tb_result->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
        } else {
            qDebug() << "Нет данных для отображения в категории 'Все'.";
        }
    }
    else if (ui->cb_category->currentText() == "Комедия" || ui->cb_category->currentText() == "Ужасы") {
        // Запрос для получения фильмов по категории через QSqlQueryModel
        QString category = ui->cb_category->currentText();
        QSqlQueryModel* model = new QSqlQueryModel(this);
        QString queryStr = QString(
                               "SELECT f.title, f.description "
                               "FROM film f "
                               "JOIN film_category fc ON f.film_id = fc.film_id "
                               "JOIN category c ON c.category_id = fc.category_id "
                               "WHERE c.name = '%1'").arg(category);

        model->setQuery(queryStr);

        if (model->rowCount() > 0) {
            // Заполняем таблицу данными
            for (int i = 0; i < model->rowCount(); ++i) {
                ui->tb_result->insertRow(i);  // Добавляем строку
                ui->tb_result->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));  // Название фильма
                ui->tb_result->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Описание фильма
            }

            // Устанавливаем заголовки столбцов
            ui->tb_result->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
        } else {
            qDebug() << "Нет данных для отображения в категории '" << category << "'.";
        }
    }
}


void MainWindow::on_pb_clear_clicked()
{
    ui->tb_result->clearContents(); // Очистить содержимое
    ui->tb_result->setRowCount(0);  // Удалить все строки
}

/*!
 * \brief Слот отображает значение в QTableWidget
 * \param widget
 * \param typeRequest
 */

void MainWindow::ScreenDataFromDB(QTableWidget* widget, int typeRequest)
{
    // Очистим таблицу
    ui->tb_result->clearContents();
    ui->tb_result->setRowCount(0);

    if (typeRequest == requestAllFilms) {
        // Для всех фильмов, получаем их через QSqlTableModel
        QSqlTableModel* model = new QSqlTableModel(this);
        model->setTable("film");
        model->setEditStrategy(QSqlTableModel::OnManualSubmit);
        model->select();

        // Заполняем таблицу данными
        for (int i = 0; i < model->rowCount(); ++i) {
            widget->insertRow(i);  // Добавляем строку
            widget->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Название фильма
            widget->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 2)).toString()));  // Описание фильма
        }

        // Заголовки столбцов
        widget->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
    }
    else if (typeRequest == requestComedy || typeRequest == requestHorrors) {
        // Для фильмов по категориям (комедия или ужасы) через QSqlQueryModel
        QSqlQueryModel* model = new QSqlQueryModel(this);
        QString category = (typeRequest == requestComedy) ? "Comedy" : "Horror";
        QString queryStr = QString(
                               "SELECT f.title, f.description "
                               "FROM film f "
                               "JOIN film_category fc ON f.film_id = fc.film_id "
                               "JOIN category c ON c.category_id = fc.category_id "
                               "WHERE c.name = '%1'").arg(category);

        model->setQuery(queryStr);

        // Заполняем таблицу данными
        for (int i = 0; i < model->rowCount(); ++i) {
            widget->insertRow(i);  // Добавляем строку
            widget->setItem(i, 0, new QTableWidgetItem(model->data(model->index(i, 0)).toString()));  // Название фильма
            widget->setItem(i, 1, new QTableWidgetItem(model->data(model->index(i, 1)).toString()));  // Описание фильма
        }

        // Заголовки столбцов
        widget->setHorizontalHeaderLabels({"Название фильма", "Описание фильма"});
    }
}



/*!
 * \brief Метод изменяет стотояние формы в зависимости от статуса подключения к БД
 * \param status
 */
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

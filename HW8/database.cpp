#include "database.h"

DataBase::DataBase(QObject *parent)
    : QObject{parent}
{

    dataBase = new QSqlDatabase();


}

DataBase::~DataBase()
{
    delete dataBase;
}

/*!
 * \brief Метод добавляет БД к экземпляру класса QSqlDataBase
 * \param driver драйвер БД
 * \param nameDB имя БД (Если отсутствует Qt задает имя по умолчанию)
 */
void DataBase::AddDataBase(QString driver, QString nameDB)
{

    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);

}

/*!
 * \brief Метод подключается к БД
 * \param для удобства передаем контейнер с данными необходимыми для подключения
 * \return возвращает тип ошибки
 */
void DataBase::ConnectToDataBase(QVector<QString> data)
{
    // Если данные для подключения не переданы, используем захардкоженные значения
    if (data.isEmpty()) {
        data.push_back("981757-ca08998.tmweb.ru");  // hostName
        data.push_back("netology_cpp");              // dbName
        data.push_back("netology_usr_cpp");          // login
        data.push_back("CppNeto3");                  // pass
        data.push_back("5432");                      // port
    }

    dataBase->setHostName(data[hostName]);
    dataBase->setDatabaseName(data[dbName]);
    dataBase->setUserName(data[login]);
    dataBase->setPassword(data[pass]);
    dataBase->setPort(data[port].toInt());

    bool status;
    status = dataBase->open();
    emit sig_SendStatusConnection(status);
}
/*!
 * \brief Метод производит отключение от БД
 * \param Имя БД
 */
void DataBase::DisconnectFromDataBase(QString nameDb)
{

    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();

}
/*!
 * \brief Метод формирует запрос к БД.
 * \param request - SQL запрос
 * \return
 */
void DataBase::RequestToDB(QString request)
{
    QTableWidget* tableWidget = new QTableWidget();  // Создаем виджет таблицы

    // Выполняем запрос
    QSqlQueryModel* model = new QSqlQueryModel();
    model->setQuery(request);

    // Подключаем данные из модели в таблицу
    tableWidget->setRowCount(model->rowCount());
    tableWidget->setColumnCount(2);  // Устанавливаем только два столбца

    // Переносим данные из модели в таблицу
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int col = 0; col < 2; ++col) {  // Обрабатываем только два столбца
            tableWidget->setItem(row, col, new QTableWidgetItem(model->data(model->index(row, col)).toString()));
        }
    }

    // Отправляем сигнал с таблицей и типом запроса
    requestType typeRequest;
    if (request.contains("film")) {
        typeRequest = requestAllFilms;
    } else if (request.contains("Comedy")) {
        typeRequest = requestComedy;
    } else if (request.contains("Horror")) {
        typeRequest = requestHorrors;
    } else {
        typeRequest = requestAllFilms;  // Если запрос не попадает в другие категории
    }

    emit sig_SendDataFromDB(tableWidget, typeRequest);  // Передаем таблицу и тип запроса
}



/*!
 * @brief Метод возвращает последнюю ошибку БД
 */
QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

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

void DataBase::AddDataBase(QString driver, QString nameDB)
{
    *dataBase = QSqlDatabase::addDatabase(driver, nameDB);
}

void DataBase::ConnectToDataBase(QVector<QString> data)
{
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

    bool status = dataBase->open();
    emit sig_SendStatusConnection(status);
}

void DataBase::DisconnectFromDataBase(QString nameDb)
{
    *dataBase = QSqlDatabase::database(nameDb);
    dataBase->close();
}

QSqlError DataBase::GetLastError()
{
    return dataBase->lastError();
}

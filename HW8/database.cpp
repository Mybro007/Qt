#include "database.h"
#include <QDebug>
#include <QSqlQuery>

DataBase::DataBase(QObject *parent)
    : QObject(parent),
    m_connectionName("QT_DB_CONN_" + QString::number(quintptr(this)))
{
    m_db = QSqlDatabase::addDatabase("QPSQL", m_connectionName);
}

DataBase::~DataBase()
{
    disconnectFromDatabase();
    QSqlDatabase::removeDatabase(m_connectionName);
}

bool DataBase::connectToDatabase(const QVector<QString>& connectionData)
{
    if (isConnected()) {
        return true;
    }

    QVector<QString> data = connectionData;
    if (data.isEmpty()) {
        data = {
            "981757-ca08998.tmweb.ru",  // host
            "netology_cpp",             // db name
            "netology_usr_cpp",         // login
            "CppNeto3",                 // password
            "5432"                      // port
        };
    }

    if (data.size() < 5) {
        qWarning() << "Invalid connection data";
        return false;
    }

    m_db.setHostName(data[0]);
    m_db.setDatabaseName(data[1]);
    m_db.setUserName(data[2]);
    m_db.setPassword(data[3]);
    m_db.setPort(data[4].toInt());

    if (!m_db.open()) {
        qWarning() << "Database connection error:" << m_db.lastError().text();
        emit connectionStatusChanged(false);
        return false;
    }

    emit connectionStatusChanged(true);
    return true;
}

void DataBase::disconnectFromDatabase()
{
    if (m_db.isOpen()) {
        m_db.close();
        emit connectionStatusChanged(false);
    }
}

bool DataBase::isConnected() const
{
    return m_db.isOpen();
}

bool DataBase::executeQuery(const QString& query, QVector<QVector<QVariant>>* result)
{
    if (!isConnected()) {
        qWarning() << "Query execution failed: database not connected";
        return false;
    }

    QSqlQuery sqlQuery(m_db);
    if (!sqlQuery.exec(query)) {
        qWarning() << "Query failed:" << sqlQuery.lastError().text();
        qWarning() << "Failed query:" << query;
        return false;
    }

    if (result) {
        result->clear();
        while (sqlQuery.next()) {
            QVector<QVariant> row;
            for (int i = 0; i < sqlQuery.record().count(); ++i) {
                row.append(sqlQuery.value(i));
            }
            result->append(row);
        }
    }

    return true;
}

bool DataBase::beginTransaction()
{
    if (!isConnected()) return false;
    return m_db.transaction();
}

bool DataBase::commitTransaction()
{
    if (!isConnected()) return false;
    return m_db.commit();
}

bool DataBase::rollbackTransaction()
{
    if (!isConnected()) return false;
    return m_db.rollback();
}

QSqlError DataBase::lastError() const
{
    return m_db.lastError();
}

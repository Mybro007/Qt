#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QSqlError>
#include <QVector>
#include <QString>
#include <QVariant>
#include <QSqlRecord>
#include <QSqlQueryModel>
#include <QSqlTableModel>

class DataBase : public QObject
{
    Q_OBJECT

public:
    explicit DataBase(QObject *parent = nullptr);
    ~DataBase();

    // Управление подключением
    bool connectToDatabase(const QVector<QString>& connectionData = {});
    void disconnectFromDatabase();
    bool isConnected() const;

    // Выполнение запросов
    bool executeQuery(const QString& query, QVector<QVector<QVariant>>* result = nullptr);
    QSqlError lastError() const;

    QSqlTableModel* getTableModel(const QString& tableName, QObject* parent = nullptr);
    QSqlQueryModel* getQueryModel(const QString& query, QObject* parent = nullptr);

    // Управление транзакциями
    bool beginTransaction();
    bool commitTransaction();
    bool rollbackTransaction();

signals:
    void connectionStatusChanged(bool connected);

private:
    QSqlDatabase m_db;
    QString m_connectionName;
};

#endif // DATABASE_H

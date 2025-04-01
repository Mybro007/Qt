#include "dbmanager.h"
#include <QDebug>

DbManager::DbManager(QObject *parent)
    : QObject(parent)
{
}

DbManager::~DbManager()
{
    m_database.disconnectFromDatabase();
}

bool DbManager::init(const QVector<QString>& connectionData)
{
    return m_database.connectToDatabase(connectionData);
}

bool DbManager::isConnected() const
{
    return m_database.isConnected();
}

QVector<QVector<QVariant>> DbManager::getAllFilms()
{
    QVector<QVector<QVariant>> result;
    QString query = "SELECT f.title, f.description "
                    "FROM film f "
                    "JOIN film_category fc ON f.film_id = fc.film_id "
                    "JOIN category c ON c.category_id = fc.category_id";

    if (!m_database.executeQuery(query, &result)) {
        emit databaseError(m_database.lastError().text());
    }
    return result;
}

QVector<QVector<QVariant>> DbManager::getFilmsByCategory(const QString& category)
{
    QVector<QVector<QVariant>> result;
    QString query = QString("SELECT f.title, f.description FROM film f "
                            "JOIN film_category fc ON f.film_id = fc.film_id "
                            "JOIN category c ON c.category_id = fc.category_id "
                            "WHERE c.name = '%1'").arg(category);

    if (!m_database.executeQuery(query, &result)) {
        emit databaseError(m_database.lastError().text());
    }
    return result;
}

bool DbManager::addFilm(const QString& title, int year, const QString& category)
{
    if (!m_database.beginTransaction()) {
        emit databaseError("Failed to start transaction");
        return false;
    }

    // В реальном приложении здесь должна быть более сложная логика
    QString query = QString("INSERT INTO film (title, release_year) "
                            "VALUES ('%1', %2)").arg(title).arg(year);

    if (!m_database.executeQuery(query)) {
        m_database.rollbackTransaction();
        emit databaseError(m_database.lastError().text());
        return false;
    }

    if (!m_database.commitTransaction()) {
        emit databaseError("Failed to commit transaction");
        return false;
    }

    return true;
}

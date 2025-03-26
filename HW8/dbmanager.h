#ifndef DBMANAGER_H
#define DBMANAGER_H

#include "database.h"
#include <QObject>

class DbManager : public QObject
{
    Q_OBJECT

public:
    explicit DbManager(QObject *parent = nullptr);
    ~DbManager();

    bool init(const QVector<QString>& connectionData = {});
    bool isConnected() const;

    // Примеры конкретных методов для работы с данными
    QVector<QVector<QVariant>> getAllFilms();
    QVector<QVector<QVariant>> getFilmsByCategory(const QString& category);
    bool addFilm(const QString& title, int year, const QString& category);

signals:
    void databaseError(const QString& error);

private:
    DataBase m_database;
};

#endif // DBMANAGER_H

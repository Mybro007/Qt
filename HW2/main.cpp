#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtNetwork/QTcpSocket>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // Создание экземпляра для работы с базой данных (QtSql)
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");

    // Создание экземпляра для работы с сетью (QtNetwork)
    QTcpSocket tcpSocket = QTcpSocket();

    return a.exec();
}

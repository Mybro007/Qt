#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qDebug() << "Доступные драйверы БД:" << QSqlDatabase::drivers();
    MainWindow w;
    w.show();
    return a.exec();
}

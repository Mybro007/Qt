#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include "database.h"
#include "dbdata.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onActAddDataTriggered();
    void onActConnectTriggered();
    void onPbRequestClicked();
    void onPbClearClicked();
    void onDatabaseDataReceived(QVector<QString> dbData);
    void onConnectionStatusChanged(bool connected);
    void handleClearRequest();
    void initTableView();
    void clearTableView();

private:
    Ui::MainWindow *ui;
    DbData *dataDb;
    DataBase *dataBase;
    QTableView *tb_result;
    QSqlTableModel* currentTableModel = nullptr;
    QSqlQueryModel* currentQueryModel = nullptr;
};

#endif // MAINWINDOW_H
